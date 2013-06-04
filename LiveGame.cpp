#include "stdafx.h"
#include "LiveGame.h"
#include "App.h"
#include "Player.h"
#include "CmdStack.h"
#include "EnumTraits.h"
#include "Players.h"
#include "Record.h"

#include <algorithm>

LiveGame::LiveGame(int id, const std::string& name, const Player& owner) : 
	Game(id, name, owner), m_phase(Phase::Lobby)
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
}

void LiveGame::AssignTeam(Player& player, RaceType race, Colour colour)
{
	GetTeam(player).Assign(race, colour);

	AdvanceTurn();
	if (m_iTurn == 0)
		StartMainPhase();
}

void LiveGame::StartMainPhase()
{
	AssertThrowModel("LiveGame::StartMainPhase", m_phase == Phase::ChooseTeam && m_iTurn == 0);

	m_phase = Phase::Main;

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
}

void LiveGame::AddCmd(CmdPtr pCmd)
{
	m_pCmdStack->AddCmd(pCmd);
}

void LiveGame::StartCmd(CmdPtr pCmd)
{
	m_pCmdStack->StartCmd(pCmd);
	
	if (GetCurrentCmd()->IsAction())
		GetCurrentTeam().GetInfluenceTrack().RemoveDiscs(1); // TODO: return these at end of round
}

Cmd* LiveGame::RemoveCmd()
{
	const Cmd* pCmd = GetCurrentCmd();
	bool bAction = pCmd && pCmd->IsAction();

	Cmd* pUndo = m_pCmdStack->RemoveCmd();

	if (bAction)
		GetCurrentTeam().GetInfluenceTrack().AddDiscs(1);

	return pUndo;
}

bool LiveGame::CanDoAction() const
{
	if (GetCurrentTeam().GetInfluenceTrack().GetDiscCount() == 0)
		return false;
	
	return !m_pCmdStack->HasAction(); // Only one action per turn.
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

void LiveGame::FinishTurn()
{
	m_pCmdStack->Clear();
	__super::FinishTurn();
}

void LiveGame::PushRecord(std::unique_ptr<Record>& pRec)
{
	m_records.push_back(std::move(pRec));
}

RecordPtr LiveGame::PopRecord()
{
	AssertThrow("LiveGame::PopRecord", !m_records.empty());
	RecordPtr pRec = std::move(m_records.back());
	m_records.pop_back();
	return pRec;
}

DEFINE_ENUM_NAMES(LiveGame::Phase) { "Lobby", "ChooseTeam", "Main", "" };
