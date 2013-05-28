#include "Input.h"
#include "App.h"
#include "Model.h"
#include "Controller.h"
#include "Output.h"
#include "Race.h"
#include "Player.h"
#include "ExploreCmd.h"
#include "InfluenceCmd.h"
#include "ColoniseCmd.h"
#include "Xml.h"
#include "EnumTraits.h"

#include <sstream>

namespace Input 
{

MessagePtr CreateCommand(const Xml::Element& root)
{
	const std::string& type = root.GetAttributeStr("type");

	if (type == "register")
		return MessagePtr(new Register(root));
	if (type == "join_game")
		return MessagePtr(new JoinGame(root));
	if (type == "exit_game")
		return MessagePtr(new ExitGame);
	if (type == "create_game")
		return MessagePtr(new CreateGame);
	if (type == "start_game")
		return MessagePtr(new StartGame);
	if (type == "choose_team")
		return MessagePtr(new ChooseTeam(root));
	if (type == "start_action")
		return MessagePtr(new StartAction(root));
	if (type == "undo")
		return MessagePtr(new Undo);
	if (type == "commit")
		return MessagePtr(new Commit);
	
	if (type == "cmd_explore_pos")
		return MessagePtr(new CmdExplorePos(root));
	if (type == "cmd_explore_hex")
		return MessagePtr(new CmdExploreHex(root));
	if (type == "cmd_explore_hex_take")
		return MessagePtr(new CmdExploreHexTake());
	if (type == "cmd_explore_discovery")
		return MessagePtr(new CmdExploreDiscovery(root));
	if (type == "cmd_colonise")
		return MessagePtr(new CmdColonisePos(root));
	if (type == "cmd_influence_src")
		return MessagePtr(new CmdInfluenceSrc(root));
	if (type == "cmd_influence_dst")
		return MessagePtr(new CmdInfluenceDst(root));
	if (type == "cmd_explore_reject")
		return MessagePtr(new CmdExploreReject);
	if (type == "cmd_colonise_pos")
		return MessagePtr(new CmdColonisePos(root));
	if (type == "cmd_colonise_squares")
		return MessagePtr(new CmdColoniseSquares(root));
	
	AssertThrow("Input command not recognised: " + type);
	return nullptr;
}

MessagePtr CreateMessage(const std::string& xml)
{
	Xml::Document doc;
	if (!doc.LoadFromString(xml))
		return false;

	return CreateCommand(doc.GetRoot());
}

Game& Message::GetGameAssert(Player& player) const
{
	Game* pGame = player.GetCurrentGame();
	AssertThrow("ChooseMessage: player not registered in any game", !!pGame);
	AssertThrow("ChooseMessage: player played out of turn", &player == &pGame->GetCurrentPlayer());
	AssertThrow("ChooseMessage: game not in main phase: " + pGame->GetName(), pGame->GetPhase() == Game::Phase::Main);
	return *pGame;
}

//-----------------------------------------------------------------------------
	
Register::Register(const Xml::Element& node) : m_idPlayer(0)
{
	node.GetAttribute("player", m_idPlayer);
	AssertThrowXML("Register", m_idPlayer > 0);
}

JoinGame::JoinGame(const Xml::Element& node) : m_idGame(0)
{
	node.GetAttribute("game", m_idGame);
	AssertThrowXML("JoinGame", m_idGame > 0);
}

namespace 
{
	void DoJoinGame(Controller& controller, Player& player, Game& game)
	{
		AssertThrow("DoJoinGame: Player already in a game: " + player.GetName(), !player.GetCurrentGame());
		player.SetCurrentGame(&game);
		if (!game.HasStarted())
		{
			game.AddTeam(player); // Might already have joined,  doesn't matter.
			controller.SendMessage(Output::UpdateLobby(game), game);
			controller.SendUpdateGameList();
		}
		controller.SendUpdateGame(game, &player);
	}
}

bool JoinGame::Process(Controller& controller, Player& player) const 
{
	Game* pGame = controller.GetModel().FindGame(m_idGame);
	AssertThrow("JoinGame: game does not exist: " + FormatInt(m_idGame), !!pGame);

	DoJoinGame(controller, player, *pGame);
	return true;
}

bool ExitGame::Process(Controller& controller, Player& player) const 
{
	Game* pGame = player.GetCurrentGame();
	AssertThrow("ExitGame: Player not in any game: " + player.GetName(), !!pGame);

	player.SetCurrentGame(nullptr);
	controller.SendMessage(Output::ShowGameList(), player);
	controller.SendUpdateGameList(&player);
	return true;
}

bool CreateGame::Process(Controller& controller, Player& player) const 
{
	Model& model = controller.GetModel();
	
	std::ostringstream ss;
	ss << "Game " <<  model.GetGames().size() + 1;
	Game& game = model.AddGame(ss.str(), player);

	DoJoinGame(controller, player, game);

	return true;	
}

bool StartGame::Process(Controller& controller, Player& player) const 
{
	Model& model = controller.GetModel();
	
	Game* pGame = player.GetCurrentGame();

	AssertThrow("StartGame: player not registered in any game", !!pGame);
	AssertThrow("StartGame: player isn't the owner of game: " + pGame->GetName(), &player == &pGame->GetOwner());
	AssertThrow("StartGame: game already started: " + pGame->GetName(), !pGame->HasStarted());
	
	pGame->StartChooseTeamPhase();

	controller.SendUpdateGameList();
	controller.SendUpdateGame(*pGame);

	return true;	
}

ChooseTeam::ChooseTeam(const Xml::Element& node)
{
	m_race = node.GetAttributeStr("race");
	m_colour = node.GetAttributeStr("colour");
}

bool ChooseTeam::Process(Controller& controller, Player& player) const 
{
	Game* pGame = player.GetCurrentGame();
	AssertThrow("ChooseTeam: player not registered in any game", !!pGame);
	AssertThrow("ChooseTeam: player played out of turn", &player == &pGame->GetCurrentPlayer());
	AssertThrow("ChooseTeam: game not in choose phase: " + pGame->GetName(), pGame->GetPhase() == Game::Phase::ChooseTeam);

	RaceType race = EnumTraits<RaceType>::FromString(m_race);
	Colour colour = EnumTraits<Colour>::FromString(m_colour);

	AssertThrowXML("ChooseTeam:race", race != RaceType::None);
	AssertThrowXML("ChooseTeam:colour", colour != Colour::None);
	AssertThrowXML("ChooseTeam: colour already taken", !pGame->GetTeamFromColour(colour));

	if (race != RaceType::Human)
		AssertThrowXML("ChooseTeam: colour doesn't match race", colour == Race(race).GetColour());

	pGame->AssignTeam(player, race, colour);

	controller.SendMessage(Output::ChooseTeam(*pGame, false), player);
	controller.SendUpdateGame(*pGame);

	return true;	
}

StartAction::StartAction(const Xml::Element& node)
{
	m_action = node.GetAttributeStr("action");
}

bool StartAction::Process(Controller& controller, Player& player) const 
{
	Game& game = GetGameAssert(player);

	if (m_action == "explore") 
		game.StartCmd(CmdPtr(new ExploreCmd(player)));
	else if (m_action == "influence") 
		game.StartCmd(CmdPtr(new InfluenceCmd(player)));
	else if (m_action == "colonise") 
		game.StartCmd(CmdPtr(new ColoniseCmd(player)));

	Cmd* pCmd = game.GetCurrentCmd();
	AssertThrow("StartAction::Process: No command created", !!pCmd);
	
	if (pCmd->IsAction())
		controller.SendMessage(Output::UpdateInfluenceTrack(*player.GetCurrentTeam()), game);

	pCmd->UpdateClient(controller);
	return true;	
}

bool Undo::Process(Controller& controller, Player& player) const 
{
	Game& game = GetGameAssert(player);

	const Cmd* pCmd = game.GetCurrentCmd();
	bool bAction = pCmd && pCmd->IsAction();

	if (Cmd* pUndo = game.RemoveCmd())
		pUndo->Undo(controller);
	
	if (Cmd* pCmd2 = game.GetCurrentCmd())
		pCmd2->UpdateClient(controller);
	else
		controller.SendMessage(Output::ChooseAction(game), player);

	if (bAction)
		controller.SendMessage(Output::UpdateInfluenceTrack(*player.GetCurrentTeam()), game);

	return true;	
}

bool Commit::Process(Controller& controller, Player& player) const 
{
	Game& game = GetGameAssert(player);

	game.FinishTurn();

	controller.SendMessage(Output::ChooseFinished(), player);
	controller.SendMessage(Output::ChooseAction(game), game.GetCurrentPlayer());
	return true;
}

//-----------------------------------------------------------------------------

bool CmdMessage::Process(Controller& controller, Player& player) const
{
	Game& game = GetGameAssert(player);
	Cmd* pCmd = game.GetCurrentCmd();
	AssertThrow("CmdMessage::Process: No current command", !!pCmd);

	CmdPtr pNext = pCmd->Process(*this, controller); // Might be null.
	game.AddCmd(std::move(pNext)); 

	if (Cmd* pNewCmd = game.GetCurrentCmd())
		pNewCmd->UpdateClient(controller);
	else
		controller.SendMessage(Output::ChooseAction(game), game.GetCurrentPlayer());

	return true;
}

CmdExplorePos::CmdExplorePos(const Xml::Element& node)
{
	m_iPos = node.GetAttributeInt("pos_idx");
}

CmdExploreHex::CmdExploreHex(const Xml::Element& node) : m_iRot(0), m_iHex(0), m_bInfluence(false)
{
	m_iRot = node.GetAttributeInt("rot_idx");
	m_iHex = node.GetAttributeInt("hex_idx");
	m_bInfluence = node.GetAttributeBool("influence");
}

CmdExploreDiscovery::CmdExploreDiscovery(const Xml::Element& node)
{
}

CmdColonisePos::CmdColonisePos(const Xml::Element& node)
{
	m_iPos = node.GetAttributeInt("pos_idx");
}

CmdColoniseSquares::CmdColoniseSquares(const Xml::Element& node)
{
	auto Read = [&] (const std::string& type, Population& pops)
	{
		auto el = node.FindFirstChild(type);
		for (auto r : EnumRange<Resource>())
			pops[r] = el.GetAttributeInt(EnumTraits<Resource>::ToString(r));
	};
	
	Read("Fixed", m_fixed);
	Read("Grey", m_grey);
	Read("Orbital", m_orbital);

	AssertThrowXML("CmdColonise: trying to add materials to orbital", m_orbital[Resource::Materials] == 0);
}

CmdInfluenceSrc::CmdInfluenceSrc(const Xml::Element& node)
{
	m_iPos = node.GetAttributeInt("pos_idx");
}

CmdInfluenceDst::CmdInfluenceDst(const Xml::Element& node)
{
	m_iPos = node.GetAttributeInt("pos_idx");
}

} // namespace