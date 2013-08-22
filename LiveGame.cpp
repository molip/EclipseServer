#include "stdafx.h"
#include "LiveGame.h"
#include "App.h"
#include "Player.h"
#include "CmdStack.h"
#include "EnumTraits.h"
#include "Players.h"
#include "Record.h"
#include "Serial.h"

#include <algorithm>

LiveGame::LiveGame() : m_phase(Phase::Lobby), m_bDoneAction(false), m_iTurn(-1), m_iRound(-1), m_iStartTeam(-1), m_iStartTeamNext(-1)
{
	m_pCmdStack = new CmdStack;
}

LiveGame::LiveGame(int id, const std::string& name, const Player& owner) : 
	Game(id, name, owner), m_phase(Phase::Lobby), m_bDoneAction(false), m_iTurn(-1), m_iRound(-1), m_iStartTeam(-1), m_iStartTeamNext(-1)
{
	m_pCmdStack = new CmdStack;
}

LiveGame::~LiveGame()
{
	delete m_pCmdStack;
}

void LiveGame::AddPlayer(Player& player)
{
	AssertThrow("LiveGame::AddTeam: Game already started: " + m_name, !HasStarted());
	if (!FindTeam(player))
		m_teams.push_back(TeamPtr(new Team(m_id, player.GetID())));
	Save();
}

void LiveGame::StartChooseTeamPhase()
{
	AssertThrow("LiveGame::Start: Game already started: " + m_name, !HasStarted());
	AssertThrow("LiveGame::Start: Game has no players: " + m_name, !m_teams.empty());
	
	m_phase = Phase::ChooseTeam;

	m_iTurn = m_iStartTeam = m_iStartTeamNext = 0;

	// Decide team order.
	std::shuffle(m_teams.begin(), m_teams.end(), GetRandom());

	//for (int i = 0; i < FakePlayers; ++i)
	//{
	//	std::ostringstream ss;
	//	ss << "Fake " << i;
	//	m_teams.insert(std::make_pair(ss.str(), nullptr));
	//	m_teamOrder.insert(m_teamOrder.begin() + i, ss.str());
	//	AssignTeam(ss.str(), RaceType::Human, Colour(i + 1));
	//}

	// Initialise hex bags.
	for (auto r : EnumRange<HexRing>())
		m_hexBag[(int)r] = HexBag(r, m_teams.size());

	m_hexBag[1].ReturnTile(206); // Discovery, no ancients, 1 materials square.

	m_repBag.Init();
	m_techBag.Init();
	m_discBag.Init();

	Save();
}

void LiveGame::AssignTeam(Player& player, RaceType race, Colour colour)
{
	GetTeam(player).Assign(race, colour);

	AdvanceTurn();
	Save();

	if (m_iTurn == 0)
		StartMainPhase();
}

void LiveGame::StartMainPhase()
{
	AssertThrowModel("LiveGame::StartMainPhase", m_phase == Phase::ChooseTeam && m_iTurn == 0);

	m_phase = Phase::Main;

	Hex& centre = m_map.AddHex(MapPos(0, 0), 001, 0);
	centre.AddShip(ShipType::GCDS, Colour::None);


	// Initialise starting hexes.
	auto startPositions = m_map.GetTeamStartPositions();
	assert(startPositions.size() == m_teams.size());
	for (size_t i = 0; i < m_teams.size(); ++i)
	{
		Team& team = *m_teams[i];
		Race r(team.GetRace());
		int idHex = r.GetStartSector(team.GetColour());

		Hex& hex = m_map.AddHex(startPositions[i], idHex, 0);

		team.PopulateStartHex(hex);
	}
	StartRound();

	Save();
}

void LiveGame::AddCmd(CmdPtr pCmd)
{
	m_pCmdStack->AddCmd(pCmd);
	Save();
}

void LiveGame::StartCmd(CmdPtr pCmd)
{
	m_pCmdStack->StartCmd(pCmd);
	
	if (GetCurrentCmd()->IsAction())
	{
		AssertThrow("LiveGame::StartCmd",  !m_bDoneAction);
		m_bDoneAction = true;
	}

	if (GetCurrentCmd()->CostsInfluence())
		GetCurrentTeam().GetInfluenceTrack().RemoveDiscs(1); // TODO: return these at end of round

	Save();
}

Cmd* LiveGame::RemoveCmd()
{
	const Cmd* pCmd = GetCurrentCmd();
	bool bAction = pCmd && pCmd->IsAction();
	bool bCostsInfluence = pCmd && pCmd->CostsInfluence();

	Cmd* pUndo = m_pCmdStack->RemoveCmd();

	if (bAction && !pUndo) // It's a start cmd.
	{
		AssertThrow("LiveGame::RemoveCmd",  m_bDoneAction);
		m_bDoneAction = false;

		if (bCostsInfluence)
			GetCurrentTeam().GetInfluenceTrack().AddDiscs(1);
	}

	Save();
	return pUndo;
}

bool LiveGame::CanDoAction() const
{
	if (GetCurrentTeam().GetInfluenceTrack().GetDiscCount() == 0)
		return false;
	
	return !m_bDoneAction; // Only one action per turn.
}

bool LiveGame::CanRemoveCmd() const
{
	return m_pCmdStack->CanRemoveCmd();
}

Cmd* LiveGame::GetCurrentCmd()
{
	return m_pCmdStack->GetCurrentCmd();
}

const Cmd* LiveGame::GetCurrentCmd() const
{
	return m_pCmdStack->GetCurrentCmd();
}

bool LiveGame::PurgeCmds()
{
	return m_pCmdStack->Purge();
}

const Player& LiveGame::GetCurrentPlayer() const
{
	return Players::Get(GetCurrentTeam().GetPlayerID());
}

Player& LiveGame::GetCurrentPlayer() 
{
	return Players::Get(GetCurrentTeam().GetPlayerID());
}

Team& LiveGame::GetCurrentTeam()
{
	return *m_teams[(m_iStartTeam + m_iTurn) % m_teams.size()];
}

void LiveGame::StartRound()
{
	assert(m_iRound < 9);

	++m_iRound;

	if (m_iRound == 9) // Game over.
		return;

	// Take new technologies from bag.
	const int startTech[] = { 12, 12, 14, 16, 18, 20 };
	const int roundTech[] = { 4, 4, 6, 7, 8, 9 };

	int nTech = (m_iRound == 0 ? startTech : roundTech)[m_teams.size() - 1];
	for (int i = 0; i < nTech && !m_techBag.IsEmpty(); ++i)
		++m_techs[m_techBag.TakeTile()];
}

void LiveGame::AdvanceTurn()
{
	m_iTurn = (m_iTurn + 1) % m_teams.size();
	Save();
}

void LiveGame::FinishTurn()
{
	m_pCmdStack->Clear();
	m_bDoneAction = false;
	AdvanceTurn();
	Save();
}

void LiveGame::PushRecord(std::unique_ptr<Record>& pRec)
{
	m_records.push_back(std::move(pRec));
	Save();
}

RecordPtr LiveGame::PopRecord()
{
	AssertThrow("LiveGame::PopRecord", !m_records.empty());
	RecordPtr pRec = std::move(m_records.back());
	m_records.pop_back();
	Save();
	return pRec;
}

void LiveGame::Save() const
{
	std::ostringstream ss;
	ss << "data/games/live/" << m_id << ".xml";
	bool bOK = Serial::SaveClass(ss.str(), *this);
	ASSERT(bOK);
}

void LiveGame::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveEnum("phase", m_phase);
	node.SaveCntr("records", m_records, Serial::ObjectSaver());
	node.SaveClass("commands", *m_pCmdStack);
	node.SaveType("done_action", m_bDoneAction);
	node.SaveType("turn", m_iTurn);
	node.SaveType("round", m_iRound);
	node.SaveType("start_team", m_iStartTeam);
	node.SaveType("start_team_next", m_iStartTeamNext);
}

void LiveGame::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadEnum("phase", m_phase);
	node.LoadCntr("records", m_records, Serial::ObjectLoader());
	node.LoadClass("commands", *m_pCmdStack);
	node.LoadType("done_action", m_bDoneAction);
	node.LoadType("turn", m_iTurn);
	node.LoadType("round", m_iRound);
	node.LoadType("start_team", m_iStartTeam);
	node.LoadType("start_team_next", m_iStartTeamNext);
}

DEFINE_ENUM_NAMES(LiveGame::Phase) { "Lobby", "ChooseTeam", "Main", "" };
