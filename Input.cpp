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
#include "ActionPhase.h"
#include "ChooseTeamPhase.h"
#include "UpkeepPhase.h"
#include "Json.h"
#include "Record.h"
#include "CommitSession.h"
#include "MessageRecord.h"

#include <sstream>

namespace Input 
{

MessagePtr CreateCommand(const Json::Element& root)
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
	if (type == "finish_upkeep")
		return MessagePtr(new FinishUpkeep);
	if (type == "chat")
		return MessagePtr(new Chat(root));
	if (type == "query_blueprint_stats")
		return MessagePtr(new QueryBlueprintStats(root));

	if (type == "cmd_explore_pos")
		return MessagePtr(new CmdExplorePos(root));
	if (type == "cmd_explore_hex")
		return MessagePtr(new CmdExploreHex(root));
	if (type == "cmd_explore_hex_take")
		return MessagePtr(new CmdExploreHexTake());
	if (type == "cmd_discovery")
		return MessagePtr(new CmdDiscovery(root));
	if (type == "cmd_influence_src")
		return MessagePtr(new CmdInfluenceSrc(root));
	if (type == "cmd_influence_flip")
		return MessagePtr(new CmdInfluenceFlip(root));
	if (type == "cmd_influence_dst")
		return MessagePtr(new CmdInfluenceDst(root));
	if (type == "cmd_abort")
		return MessagePtr(new CmdAbort);
	if (type == "cmd_colonise_pos")
		return MessagePtr(new CmdColonisePos(root));
	if (type == "cmd_colonise_squares")
		return MessagePtr(new CmdColoniseSquares(root));
	if (type == "cmd_uncolonise")
		return MessagePtr(new CmdUncolonise(root));
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
	if (type == "cmd_combat")
		return MessagePtr(new CmdCombat(root));
	if (type == "cmd_dice")
		return MessagePtr(new CmdDice(root));

	VERIFY_INPUT_MSG(type, false);
	return nullptr;
}

MessagePtr CreateMessage(const std::string& msg)
{
	Json::Document jsonDoc;
	if (!jsonDoc.LoadFromString(msg))
		return false;
	
	return CreateCommand(jsonDoc);
}

const LiveGame& Message::GetLiveGame(const Player& player) const
{
	const LiveGame* pGame = player.GetCurrentLiveGame();
	VERIFY_INPUT_MSG(player.GetName(), !!pGame);
	VERIFY_INPUT_MSG(pGame->GetName(), pGame->GetGamePhase() == LiveGame::GamePhase::Main);
	return *pGame;
}

//-----------------------------------------------------------------------------
	
Register::Register(const Json::Element& node) : m_idPlayer(0)
{
	node.GetAttribute("player", m_idPlayer);
	VERIFY_INPUT(m_idPlayer > 0);
}

JoinGame::JoinGame(const Json::Element& node) : m_idGame(0)
{
	node.GetAttribute("game", m_idGame);
	VERIFY_INPUT(m_idGame > 0);
}

namespace 
{
	void DoJoinGame(Controller& controller, Player& player, const LiveGame& game)
	{
		VERIFY_INPUT_MSG(player.GetName(), !player.GetCurrentGame());
		player.SetCurrentGame(&game);
		if (!game.HasStarted())
		{
			// Might already have joined,  doesn't matter.
			CommitSession session(game, controller);
			session.Open().AddPlayer(player);
			session.Commit();
			
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
	const Game* pGame = player.GetCurrentGame();
	VERIFY_INPUT_MSG(player.GetName(), !!pGame);

	player.SetCurrentGame(nullptr);
	controller.SendMessage(Output::ShowGameList(), player);
	controller.SendUpdateGameList(&player);
	return true;
}

bool StartReview::Process(Controller& controller, Player& player) const 
{
	const LiveGame* pLive = player.GetCurrentLiveGame();
	VERIFY_INPUT_MSG(player.GetName(), !!pLive);

	ReviewGame& review = Games::AddReview(player, *pLive);
	
	player.SetCurrentGame(&review);
	controller.SendUpdateGame(review, &player);
	return true;
}

bool ExitReview::Process(Controller& controller, Player& player) const 
{
	const ReviewGame* pReview = player.GetCurrentReviewGame();
	VERIFY_INPUT_MSG(player.GetName(), !!pReview);

	const LiveGame& live = Games::GetLive(pReview->GetLiveGameID());
	player.SetCurrentGame(&live);

	Games::DeleteReview(pReview->GetID());
	
	controller.SendUpdateGame(live, &player);
	return true;
}

bool AdvanceReview::Process(Controller& controller, Player& player) const 
{
	const ReviewGame* pReview = player.GetCurrentReviewGame();
	VERIFY_INPUT_MSG(player.GetName(), !!pReview);

	Record::DoImmediate(*pReview, [&](ReviewGame& game) { game.Advance(controller); });

	controller.SendMessage(Output::UpdateReviewUI(*pReview), player);
	return true;
}

bool RetreatReview::Process(Controller& controller, Player& player) const 
{
	const ReviewGame* pReview = player.GetCurrentReviewGame();
	VERIFY_INPUT_MSG(player.GetName(), !!pReview);
	Record::DoImmediate(*pReview, [&](ReviewGame& game) { game.Retreat(controller); });

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
	const LiveGame* pGame = player.GetCurrentLiveGame();

	VERIFY_INPUT_MSG(player.GetName(), !!pGame);
	VERIFY_INPUT_MSG(player.GetName(), &player == &pGame->GetOwner());
	VERIFY_INPUT_MSG(pGame->GetName(), !pGame->HasStarted());
	
	CommitSession session(*pGame, controller);
	session.Open().StartChooseTeamGamePhase();
	session.Commit();

	controller.SendUpdateGameList();
	controller.SendUpdateGame(*pGame);

	return true;	
}

ChooseTeam::ChooseTeam(const Json::Element& node)
{
	m_race = node.GetAttributeStr("race");
	m_colour = node.GetAttributeStr("colour");
}

bool ChooseTeam::Process(Controller& controller, Player& player) const 
{
	const LiveGame* pGame = player.GetCurrentLiveGame();
	VERIFY_INPUT_MSG(player.GetName(), !!pGame);
	VERIFY_INPUT_MSG(player.GetName(),
		pGame->GetChooseTeamPhase().GetCurrentTeam().GetColour() == player.GetCurrentTeam()->GetColour());

	RaceType race = ::EnumFromString<RaceType>(m_race);
	Colour colour = ::EnumFromString<Colour>(m_colour);

	VERIFY_INPUT_MSG("race", race != RaceType::None);
	VERIFY_INPUT_MSG("colour", colour != Colour::None);
	VERIFY_INPUT_MSG("colour already taken", !pGame->FindTeam(colour));

	if (race != RaceType::Human)
		VERIFY_INPUT_MSG("colour doesn't match race", colour == Race(race).GetColour());

	CommitSession session(*pGame, controller);
	session.Open().GetChooseTeamPhase().AssignTeam(session, player, race, colour);
	session.Commit();

	return true;	
}

StartAction::StartAction(const Json::Element& node)
{
	m_action = node.GetAttributeStr("action");
}

bool StartAction::Process(Controller& controller, Player& player) const 
{
	const LiveGame& game = GetLiveGame(player);
	Colour colour = game.GetTeam(player).GetColour();

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

	VERIFY_INPUT_MSG("No command created", !!pCmd);
	
	CommitSession session(game, controller);
	session.Open().GetPhase().StartCmd(CmdPtr(pCmd), session);
	session.Commit();

	return true;	
}

bool Undo::Process(Controller& controller, Player& player) const 
{
	CommitSession session(GetLiveGame(player), controller);
	session.Open().GetPhase().UndoCmd(session, player);
	session.Commit();
	return true;	
}

bool Commit::Process(Controller& controller, Player& player) const 
{
	CommitSession session(GetLiveGame(player), controller);
	session.Open().GetActionPhase().FinishTurn(session);
	session.Commit();
	return true;
}

bool FinishUpkeep::Process(Controller& controller, Player& player) const
{
	CommitSession session(GetLiveGame(player), controller);
	session.Open().GetUpkeepPhase().FinishUpkeep(session, player);
	session.Commit();
	return true;
}

Chat::Chat(const Json::Element& node)
{
	m_msg = node.GetAttributeStr("message");
}

bool Chat::Process(Controller& controller, Player& player) const
{
	CommitSession session(GetLiveGame(player), controller);
	
	Colour colour = Colour::None;
	if (const Team* team = player.GetCurrentTeam())
		colour = team->GetColour();

	session.DoAndPushRecord(RecordPtr(new MessageRecord(player.GetName(), colour, m_msg)));
	session.Commit();
	return true;
}

SlotChanges::SlotChanges(const Json::Array& node)
{
	for (Json::Element child : node)
	{
		ShipType ship = ShipType(child.GetAttributeInt("ship"));
		int slot = child.GetAttributeInt("slot");
		ShipPart part = ::EnumFromString<ShipPart>(child.GetAttributeStr("part"));

		push_back(SlotChange{ ship, slot, part });
	}
}

QueryBlueprintStats::QueryBlueprintStats(const Json::Element& node) : m_changes(node.GetChildArray("changes"))
{
}

bool QueryBlueprintStats::Process(Controller& controller, Player& player) const
{
	return true;
}

//-----------------------------------------------------------------------------

bool CmdMessage::Process(Controller& controller, Player& player) const
{
	CommitSession session(GetLiveGame(player), controller);
	session.Open().GetPhase().ProcessCmdMessage(*this, session, player);
	session.Commit();
	return true;
}

CmdExplorePos::CmdExplorePos(const Json::Element& node)
{
	m_iPos = node.GetAttributeInt("pos_idx");
}

CmdExploreHex::CmdExploreHex(const Json::Element& node) : m_iRot(0), m_iHex(0), m_bInfluence(false)
{
	m_iRot = node.GetAttributeInt("rot_idx");
	m_iHex = node.GetAttributeInt("hex_idx");
	m_bInfluence = node.GetAttributeBool("influence");
}

CmdDiscovery::CmdDiscovery(const Json::Element& node)
{
	std::string action = node.GetAttributeStr("action");
	m_action =
		action == "points" ? Action::Points :
		action == "use" ? Action::Use :
		action == "keep" ? Action::Keep :
		Action::None;
}

CmdColonisePos::CmdColonisePos(const Json::Element& node)
{
	m_iPos = node.GetAttributeInt("pos_idx");
}

CmdColoniseBase::CmdColoniseBase(const Json::Element& node)
{
	for (auto r : EnumRange<Resource>())
		m_moved[r] = node.GetChildElement("moved").GetAttributeInt(::EnumToString(r));
}

CmdColoniseSquares::CmdColoniseSquares(const Json::Element& node) : CmdColoniseBase(node)
{
}

CmdUncolonise::CmdUncolonise(const Json::Element& node) : CmdColoniseBase(node)
{
}

CmdInfluenceSrc::CmdInfluenceSrc(const Json::Element& node)
{
	m_iPos = node.GetAttributeInt("pos_idx");
}

CmdInfluenceFlip::CmdInfluenceFlip(const Json::Element& node)
{
}

CmdInfluenceDst::CmdInfluenceDst(const Json::Element& node)
{
	m_iPos = node.GetAttributeInt("pos_idx");
}

CmdResearch::CmdResearch(const Json::Element& node)
{
	m_iTech = node.GetAttributeInt("tech_idx");
}

CmdResearchArtifact::CmdResearchArtifact(const Json::Element& node)
{
	for (auto r : EnumRange<Resource>())
		m_artifacts[r] = node.GetAttributeInt(::EnumToString(r));
}

CmdMoveSrc::CmdMoveSrc(const Json::Element& node)
{
	m_x = node.GetAttributeInt("x");
	m_y = node.GetAttributeInt("y");
	m_ship = ::EnumFromString<ShipType>(node.GetAttributeStr("ship"));
}

CmdMoveDst::CmdMoveDst(const Json::Element& node)
{
	m_x = node.GetAttributeInt("x");
	m_y = node.GetAttributeInt("y");
}

CmdBuild::CmdBuild(const Json::Element& node)
{
	m_x = node.GetAttributeInt("x");
	m_y = node.GetAttributeInt("y");
	m_buildable = ::EnumFromString<Buildable>(node.GetAttributeStr("buildable"));
}

CmdDiplomacy::CmdDiplomacy(const Json::Element& node)
{
}

CmdUpgrade::CmdUpgrade(const Json::Element& node) : m_changes(node.GetChildArray("changes"))
{
}

CmdTrade::CmdTrade(const Json::Element& node)
{
	m_from = ::EnumFromString<Resource>(node.GetAttributeStr("from"));
	m_to = ::EnumFromString<Resource>(node.GetAttributeStr("to"));
	m_count = node.GetAttributeInt("count");
}

CmdCombat::CmdCombat(const Json::Element& node)
{
	m_fire = node.GetAttributeBool("fire");
}

CmdDice::CmdDice(const Json::Element& node)
{

}
} // namespace