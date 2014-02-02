#include "Input.h"
#include "App.h"
#include "Controller.h"
#include "Output.h"
#include "Race.h"
#include "Player.h"
#include "ExploreCmd.h"
#include "InfluenceCmd.h"
#include "ColoniseCmd.h"
#include "ResearchCmd.h"
#include "MoveCmd.h"
#include "BuildCmd.h"
#include "DiplomacyCmd.h"
#include "UpgradeCmd.h"
#include "TradeCmd.h"
#include "PassCmd.h"
#include "Xml.h"
#include "EnumTraits.h"
#include "Games.h"
#include "LiveGame.h"
#include "ReviewGame.h"
#include "Record.h"
#include "ActionPhase.h"
#include "ChooseTeamPhase.h"

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
	if (type == "start_review")
		return MessagePtr(new StartReview);
	if (type == "exit_review")
		return MessagePtr(new ExitReview);
	if (type == "advance_review")
		return MessagePtr(new AdvanceReview);
	if (type == "retreat_review")
		return MessagePtr(new RetreatReview);
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
	if (type == "cmd_abort")
		return MessagePtr(new CmdAbort);
	if (type == "cmd_colonise_pos")
		return MessagePtr(new CmdColonisePos(root));
	if (type == "cmd_colonise_squares")
		return MessagePtr(new CmdColoniseSquares(root));
	if (type == "cmd_research")
		return MessagePtr(new CmdResearch(root));
	if (type == "cmd_research_artifact")
		return MessagePtr(new CmdResearchArtifact(root));
	if (type == "cmd_move_src")
		return MessagePtr(new CmdMoveSrc(root));
	if (type == "cmd_move_dst")
		return MessagePtr(new CmdMoveDst(root));
	if (type == "cmd_build")
		return MessagePtr(new CmdBuild(root));
	if (type == "cmd_upgrade")
		return MessagePtr(new CmdUpgrade(root));
	if (type == "cmd_diplomacy")
		return MessagePtr(new CmdDiplomacy(root));
	if (type == "cmd_trade")
		return MessagePtr(new CmdTrade(root));
	
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

LiveGame& Message::GetLiveGame(Player& player) const
{
	LiveGame* pGame = player.GetCurrentLiveGame();
	AssertThrow("ChooseMessage: player not registered in any game", !!pGame);
	AssertThrow("ChooseMessage: game not in main phase: " + pGame->GetName(), pGame->GetGamePhase() == LiveGame::GamePhase::Main);
	AssertThrow("ChooseMessage: player played out of turn", pGame->GetPhase().IsTeamActive(player.GetCurrentTeam()->GetColour()));
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
	void DoJoinGame(Controller& controller, Player& player, LiveGame& game)
	{
		AssertThrow("DoJoinGame: Player already in a game: " + player.GetName(), !player.GetCurrentGame());
		player.SetCurrentGame(&game);
		if (!game.HasStarted())
		{
			game.AddPlayer(player); // Might already have joined,  doesn't matter.
			controller.SendMessage(Output::UpdateLobby(game), game);
			controller.SendUpdateGameList();
		}
		controller.SendUpdateGame(game, &player);
	}
}

bool JoinGame::Process(Controller& controller, Player& player) const 
{
	DoJoinGame(controller, player, Games::GetLive(m_idGame));
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

bool StartReview::Process(Controller& controller, Player& player) const 
{
	LiveGame* pLive = player.GetCurrentLiveGame();
	AssertThrow("ExitGame: Player not in live game: " + player.GetName(), !!pLive);

	ReviewGame& review = Games::AddReview(player, *pLive);
	
	player.SetCurrentGame(&review);
	controller.SendUpdateGame(review, &player);
	return true;
}

bool ExitReview::Process(Controller& controller, Player& player) const 
{
	ReviewGame* pReview = player.GetCurrentReviewGame();
	AssertThrow("ExitGame: Player not in review game: " + player.GetName(), !!pReview);

	LiveGame& live = Games::GetLive(pReview->GetLiveGameID());
	player.SetCurrentGame(&live);

	Games::DeleteReview(pReview->GetID());
	
	controller.SendUpdateGame(live, &player);
	return true;
}

bool AdvanceReview::Process(Controller& controller, Player& player) const 
{
	ReviewGame* pReview = player.GetCurrentReviewGame();
	AssertThrow("AdvanceReview: Player not in review game: " + player.GetName(), !!pReview);
	pReview->Advance(controller);

	controller.SendMessage(Output::UpdateReviewUI(*pReview), player);
	return true;
}

bool RetreatReview::Process(Controller& controller, Player& player) const 
{
	ReviewGame* pReview = player.GetCurrentReviewGame();
	AssertThrow("RetreatReview: Player not in review game: " + player.GetName(), !!pReview);
	pReview->Retreat(controller);
	
	controller.SendMessage(Output::UpdateReviewUI(*pReview), player);
	return true;
}

bool CreateGame::Process(Controller& controller, Player& player) const 
{
	std::ostringstream ss;
	ss << "Game " <<  Games::GetLiveGames().size() + 1;
	LiveGame& game = Games::Add(ss.str(), player);

	DoJoinGame(controller, player, game);

	return true;	
}

bool StartGame::Process(Controller& controller, Player& player) const 
{
	LiveGame* pGame = player.GetCurrentLiveGame();

	AssertThrow("StartGame: player not registered in any game", !!pGame);
	AssertThrow("StartGame: player isn't the owner of game: " + pGame->GetName(), &player == &pGame->GetOwner());
	AssertThrow("StartGame: game already started: " + pGame->GetName(), !pGame->HasStarted());
	
	pGame->StartChooseTeamGamePhase();

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
	LiveGame* pGame = player.GetCurrentLiveGame();
	AssertThrow("ChooseTeam: player not registered in any game", !!pGame);
	AssertThrow("ChooseMessage: player played out of turn", 
		pGame->GetChooseTeamPhase().GetCurrentTeam().GetColour() == player.GetCurrentTeam()->GetColour());

	RaceType race = EnumTraits<RaceType>::FromString(m_race);
	Colour colour = EnumTraits<Colour>::FromString(m_colour);

	AssertThrowXML("ChooseTeam:race", race != RaceType::None);
	AssertThrowXML("ChooseTeam:colour", colour != Colour::None);
	AssertThrowXML("ChooseTeam: colour already taken", !pGame->FindTeam(colour));

	if (race != RaceType::Human)
		AssertThrowXML("ChooseTeam: colour doesn't match race", colour == Race(race).GetColour());

	pGame->GetChooseTeamPhase().AssignTeam(player, race, colour);

	controller.SendMessage(Output::ChooseTeam(*pGame, false), player);
	controller.SendUpdateGame(*pGame);

	if (pGame->GetGamePhase() == LiveGame::GamePhase::Main)
		controller.SendMessage(Output::UpdateTechnologies(*pGame), *pGame); // TODO: Move to record.

	return true;	
}

StartAction::StartAction(const Xml::Element& node)
{
	m_action = node.GetAttributeStr("action");
}

bool StartAction::Process(Controller& controller, Player& player) const 
{
	LiveGame& game = GetLiveGame(player);
	Colour colour = player.GetCurrentTeam()->GetColour();

	Cmd* pCmd = nullptr;

	if (m_action == "explore") 
		pCmd = new ExploreCmd(colour, game);
	else if (m_action == "influence") 
		pCmd = new InfluenceCmd(colour, game);
	else if (m_action == "colonise") 
		pCmd = new ColoniseCmd(colour, game);
	else if (m_action == "research") 
		pCmd = new ResearchCmd(colour, game);
	else if (m_action == "move") 
		pCmd = new MoveCmd(colour, game);
	else if (m_action == "build") 
		pCmd = new BuildCmd(colour, game);
	else if (m_action == "diplomacy") 
		pCmd = new DiplomacyCmd(colour, game);
	else if (m_action == "upgrade") 
		pCmd = new UpgradeCmd(colour, game);
	else if (m_action == "trade") 
		pCmd = new TradeCmd(colour, game);
	else if (m_action == "pass") 
		pCmd = new PassCmd(colour, game);

	AssertThrow("StartAction::Process: No command created", !!pCmd);
	
	game.GetActionPhase().StartCmd(CmdPtr(pCmd), controller);

	return true;	
}

bool Undo::Process(Controller& controller, Player& player) const 
{
	GetLiveGame(player).GetPhase().UndoCmd(controller, player); 
	return true;	
}

bool Commit::Process(Controller& controller, Player& player) const 
{
	GetLiveGame(player).GetActionPhase().FinishTurn(controller);
	return true;
}

//-----------------------------------------------------------------------------

bool CmdMessage::Process(Controller& controller, Player& player) const
{
	GetLiveGame(player).GetPhase().ProcessCmdMessage(*this, controller, player);
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
		auto el = node.GetFirstChild(type);
		AssertThrowXML("CmdColoniseSquares: child not found", !el.IsNull());
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

CmdResearch::CmdResearch(const Xml::Element& node)
{
	m_iTech = node.GetAttributeInt("tech_idx");
}

CmdResearchArtifact::CmdResearchArtifact(const Xml::Element& node)
{
	for (auto r : EnumRange<Resource>())
		m_artifacts[r] = node.GetAttributeInt(EnumTraits<Resource>::ToString(r));
}

CmdMoveSrc::CmdMoveSrc(const Xml::Element& node)
{
	m_x = node.GetAttributeInt("x");
	m_y = node.GetAttributeInt("y");
	m_ship = EnumTraits<ShipType>::FromString(node.GetAttributeStr("ship"));
}

CmdMoveDst::CmdMoveDst(const Xml::Element& node)
{
	m_x = node.GetAttributeInt("x");
	m_y = node.GetAttributeInt("y");
}

CmdBuild::CmdBuild(const Xml::Element& node)
{
	m_x = node.GetAttributeInt("x");
	m_y = node.GetAttributeInt("y");
	m_buildable = EnumTraits<Buildable>::FromString(node.GetAttributeStr("buildable"));
}

CmdDiplomacy::CmdDiplomacy(const Xml::Element& node)
{
}

CmdUpgrade::CmdUpgrade(const Xml::Element& node)
{
}

CmdTrade::CmdTrade(const Xml::Element& node)
{
}

} // namespace