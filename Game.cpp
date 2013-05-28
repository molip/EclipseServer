#include "stdafx.h"
#include "Game.h"
#include "App.h"
#include "Player.h"
#include "CmdStack.h"
#include "EnumTraits.h"
#include "Players.h"

#include <algorithm>

namespace
{
	const int FakePlayers = 3;
}

Game::Game(int id, const std::string& name, Player& owner) : 
	m_id(id), m_name(name), m_owner(owner), m_phase(Phase::Lobby), m_iTurn(-1), m_iRound(-1), m_iStartTeam(-1), m_iStartTeamNext(-1),
	m_map(*this)
{
	m_pCmdStack = new CmdStack;
}

Game::~Game()
{
	delete m_pCmdStack;
}

void Game::AddPlayer(Player& player)
{
	AssertThrow("Game::AddTeam: Game already started: " + m_name, !HasStarted());
	if (!FindTeam(player))
		m_teams.push_back(TeamPtr(new Team(m_id, player.GetID())));
}

void Game::StartChooseTeamPhase()
{
	AssertThrow("Game::Start: Game already started: " + m_name, !HasStarted());
	AssertThrow("Game::Start: Game has no players: " + m_name, !m_teams.empty());
	
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

void Game::StartMainPhase()
{
	AssertThrowModel("Game::StartMainPhase", m_phase == Phase::ChooseTeam && m_iTurn == 0);

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

void Game::StartRound()
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
		m_techs.insert(m_techBag.TakeTile());
}

void Game::AssertStarted() const
{
	AssertThrow("Game not started yet: " + m_name, HasStarted());
}

bool Game::HasTeamChosen(const Team& team) const
{
	return team.GetRace() != RaceType::None;
}

Team* Game::FindTeam(const Player& player)
{
	for (auto& t : m_teams)
		if (t->GetPlayerID() == player.GetID())
			return t.get();
	return nullptr;
}

Team& Game::GetTeam(const Player& player)
{
	Team* pTeam = FindTeam(player);
	AssertThrow("Game::GetTeam: player not in game: " + player.GetName(), !!pTeam);
	return *pTeam;
}

const Player& Game::GetCurrentPlayer() const
{
	return Players::Get(GetCurrentTeam().GetPlayerID());
}

Player& Game::GetCurrentPlayer() 
{
	return Players::Get(GetCurrentTeam().GetPlayerID());
}

Team& Game::GetCurrentTeam()
{
	AssertStarted();
	return *m_teams[(m_iStartTeam + m_iTurn) % m_teams.size()];
}

const Team* Game::FindTeam(Colour c) const
{
	for (auto& t : m_teams)
		if (t->GetColour() == c)
			return t.get();
	return nullptr;
}

const Team& Game::GetTeam(Colour c) const
{
	const Team* pTeam = FindTeam(c);
	AssertThrow("Game::GetTeam: colour not in game: ", !!pTeam);
	return *pTeam;
}

void Game::AssignTeam(Player& player, RaceType race, Colour colour)
{
	GetTeam(player).Assign(race, colour);

	AdvanceTurn();
	if (m_iTurn == 0)
		StartMainPhase();
}

void Game::HaveTurn(Player& player)
{
	AdvanceTurn();
}

void Game::AdvanceTurn()
{
	m_iTurn = (m_iTurn + 1) % m_teams.size();
}

void Game::AddCmd(CmdPtr pCmd)
{
	m_pCmdStack->AddCmd(pCmd);
}

void Game::StartCmd(CmdPtr pCmd)
{
	m_pCmdStack->StartCmd(pCmd);
	
	if (GetCurrentCmd()->IsAction())
		GetCurrentTeam().GetInfluenceTrack().RemoveDiscs(1); // TODO: return these at end of round
}

Cmd* Game::RemoveCmd()
{
	const Cmd* pCmd = GetCurrentCmd();
	bool bAction = pCmd && pCmd->IsAction();

	Cmd* pUndo = m_pCmdStack->RemoveCmd();

	if (bAction)
		GetCurrentTeam().GetInfluenceTrack().AddDiscs(1);

	return pUndo;
}

bool Game::CanDoAction() const
{
	if (GetCurrentTeam().GetInfluenceTrack().GetDiscCount() == 0)
		return false;
	
	return !m_pCmdStack->HasAction(); // Only one action per turn.
}

bool Game::CanRemoveCmd() const
{
	return m_pCmdStack->CanRemoveCmd();
}

Cmd* Game::GetCurrentCmd()
{
	return m_pCmdStack->GetCurrentCmd();
}

const Cmd* Game::GetCurrentCmd() const
{
	return m_pCmdStack->GetCurrentCmd();
}

void Game::FinishTurn()
{
	m_pCmdStack->Clear();
	AdvanceTurn();
}

DEFINE_ENUM_NAMES(Game::Phase) { "Lobby", "ChooseTeam", "Main", "" };
