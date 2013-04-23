#include "Input.h"
#include "App.h"
#include "Model.h"
#include "Controller.h"
#include "Output.h"
#include "Race.h"
#include "Player.h"
#include "ExploreCmd.h"
#include "InfluenceCmd.h"

#include <sstream>

namespace Input 
{

RaceType GetRaceFromName(const std::string& race)
{
	if (race == "eridani") return RaceType::Eridani;
	if (race == "hydran") return RaceType::Hydran;
	if (race == "planta") return RaceType::Planta;
	if (race == "descendants") return RaceType::Descendants;
	if (race == "mechanema") return RaceType::Mechanema;
	if (race == "orion") return RaceType::Orion;
	if (race == "human") return RaceType::Human;

	return RaceType::None;
}

Colour GetColourFromName(const std::string& colour)
{
	if (colour == "black") return Colour::Black;
	if (colour == "blue") return Colour::Blue;
	if (colour == "green") return Colour::Green;
	if (colour == "red") return Colour::Red;
	if (colour == "white") return Colour::White;
	if (colour == "yellow") return Colour::Yellow;

	return Colour::None;
}

MessagePtr CreateCommand(const std::string& type, const TiXmlElement& root)
{
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
	//if (type == "commit")
	//	return MessagePtr(new Commit);
	
	if (type == "cmd_explore_pos")
		return MessagePtr(new CmdExplorePos(root));
	if (type == "cmd_explore_hex")
		return MessagePtr(new CmdExploreHex(root));
	if (type == "cmd_explore_discovery")
		return MessagePtr(new CmdExploreDiscovery(root));
	if (type == "cmd_influence_src")
		return MessagePtr(new CmdInfluenceSrc(root));
	if (type == "cmd_influence_dst")
		return MessagePtr(new CmdInfluenceDst(root));
	if (type == "cmd_explore_reject")
		return MessagePtr(new CmdExploreReject);
	
	AssertThrow("Input command not recognised: " + type);
	return nullptr;
}

MessagePtr CreateMessage(const std::string& xml)
{
	XmlDoc doc;
	doc.Parse(xml.c_str());
	if (doc.Error())
		return false;

	if (TiXmlElement* pRoot = doc.RootElement())
		if (auto pTypeStr = pRoot->Attribute("type"))
			return CreateCommand(pTypeStr, *pRoot);
	
	return nullptr;
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
	
Register::Register(const TiXmlElement& node) : m_idPlayer(0)
{
	node.Attribute("player", &m_idPlayer);
	AssertThrowXML("Register", m_idPlayer > 0);
}

JoinGame::JoinGame(const TiXmlElement& node) : m_idGame(0)
{
	node.Attribute("game", &m_idGame);
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

ChooseTeam::ChooseTeam(const TiXmlElement& node)
{
	m_race = node.Attribute("race");
	m_colour = node.Attribute("colour");
}

bool ChooseTeam::Process(Controller& controller, Player& player) const 
{
	Game* pGame = player.GetCurrentGame();
	AssertThrow("ChooseTeam: player not registered in any game", !!pGame);
	AssertThrow("ChooseTeam: player played out of turn", &player == &pGame->GetCurrentPlayer());
	AssertThrow("ChooseTeam: game not in choose phase: " + pGame->GetName(), pGame->GetPhase() == Game::Phase::ChooseTeam);

	RaceType race = GetRaceFromName(m_race);
	Colour colour = GetColourFromName(m_colour);

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

StartAction::StartAction(const TiXmlElement& node)
{
	m_action = node.Attribute("action");
}

bool StartAction::Process(Controller& controller, Player& player) const 
{
	Game& game = GetGameAssert(player);

	if (m_action == "explore") 
		game.StartCmd(CmdPtr(new ExploreCmd(game, player)));
	else if (m_action == "influence") 
		game.StartCmd(CmdPtr(new InfluenceCmd(game, player)));

	Cmd* pCmd = game.GetCurrentCmd();
	AssertThrow("StartAction::Process: No command created", !!pCmd);
	
	pCmd->UpdateClient(controller);
	return true;	
}

bool Undo::Process(Controller& controller, Player& player) const 
{
	Game& game = GetGameAssert(player);

	Cmd* pCmd = game.GetCurrentCmd();
	AssertThrow("Undo::Process: No command to undo", !!pCmd);
	AssertThrow("Undo::Process: Can't undo", pCmd->CanUndo());

	if (pCmd->Undo()) // Delete cmd
	{
		game.AbortCurrentCmd();
		controller.SendMessage(Output::ChooseAction(), player);
	}
	else
	{
		pCmd->UpdateClient(controller);
	}

	return true;	
}

//bool Commit::Process(Controller& controller, Player& player) const 
//{
//	Game& game = GetGameAssert(player);
//	game.CommitCurrentCmd();
//
//	controller.SendMessage(Output::ChooseFinished(), player);
//	controller.SendMessage(Output::ChooseAction(), game.GetCurrentPlayer());
//	return true;
//}

//-----------------------------------------------------------------------------

bool CmdMessage::Process(Controller& controller, Player& player) const
{
	Game& game = GetGameAssert(player);
	Cmd* pCmd = game.GetCurrentCmd();
	AssertThrow("CmdMessage::Process: No current command", !!pCmd);

	pCmd->AcceptMessage(*this);
	pCmd->UpdateClient(controller);
	
	if (pCmd->IsFinished())
	{
		controller.SendMessage(Output::ChooseFinished(), player);
		game.CommitCurrentCmd();
		controller.SendMessage(Output::ChooseAction(), game.GetCurrentPlayer());
	}
	return true;
}

CmdExplorePos::CmdExplorePos(const TiXmlElement& node) : m_iPos(-1)
{
	AssertThrowXML("CmdExplorePos: pos_idx", !!node.Attribute("pos_idx", &m_iPos));
}

CmdExploreHex::CmdExploreHex(const TiXmlElement& node) : m_iRot(0), m_iHex(0), m_bInfluence(false)
{
	AssertThrowXML("CmdExploreHex: rot_idx", !!node.Attribute("rot_idx", &m_iRot));
	AssertThrowXML("CmdExploreHex: hex_idx", !!node.Attribute("hex_idx", &m_iHex));
	AssertThrowXML("CmdExploreHex: influence", node.QueryBoolAttribute("influence", &m_bInfluence) == TIXML_SUCCESS);
}

CmdExploreDiscovery::CmdExploreDiscovery(const TiXmlElement& node)
{
}

CmdInfluenceSrc::CmdInfluenceSrc(const TiXmlElement& node) : m_iPos(-1)
{
	AssertThrowXML("CmdInfluenceSrc: pos_idx", !!node.Attribute("pos_idx", &m_iPos));
}

CmdInfluenceDst::CmdInfluenceDst(const TiXmlElement& node) : m_iPos(-1)
{
	AssertThrowXML("CmdInfluenceDst: pos_idx", !!node.Attribute("pos_idx", &m_iPos));
}

} // namespace