#include "stdafx.h"
#include "Game.h"
#include "App.h"
#include "Player.h"

#include <algorithm>

namespace
{
	const int FakePlayers = 3;
}

Game::Game(const std::string& name, Player& owner) : 
	m_name(name), m_owner(owner), m_phase(Phase::Lobby), m_iTurn(-1), m_iRound(-1), m_iStartTeam(-1), m_iStartTeamNext(-1),
	m_map(*this)
{
}

bool Game::AddTeam(Player& player)
{
	AssertThrow("Game::AddTeam: Game already started: " + m_name, !HasStarted());
	return m_teams.insert(std::make_pair(&player, nullptr)).second; // OK if already added.
}

void Game::StartChooseTeamPhase()
{
	AssertThrow("Game::Start: Game already started: " + m_name, !HasStarted());
	AssertThrow("Game::Start: Game has no players: " + m_name, !m_teams.empty());
	
	m_phase = Phase::ChooseTeam;

	m_iTurn = m_iStartTeam = m_iStartTeamNext = 0;

	// Decide team order.
	for (auto& i : m_teams)
		m_teamOrder.push_back(i.first);
	std::random_shuffle(m_teamOrder.begin(), m_teamOrder.end());

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
		m_hexBag[(int)r] = HexBag(r, m_teamOrder.size());
}

void Game::StartMainPhase()
{
	AssertThrowModel("Game::StartMainPhase", m_phase == Phase::ChooseTeam && m_iTurn == 0);

	m_phase = Phase::Main;

	// Initialise starting hexes.
	auto startPositions = m_map.GetTeamStartPositions();
	assert(startPositions.size() == m_teamOrder.size());
	for (size_t i = 0; i < m_teamOrder.size(); ++i)
	{
		Team& team = GetTeam(*m_teamOrder[i]);
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

bool Game::HasTeamChosen(Player& player) const
{
	auto i = m_teams.find(&player);
	AssertThrow("Game::HasTeamChosen: Team not found: " + player.GetName(), i != m_teams.end());
	return i->second.get() != nullptr;
}

const Team& Game::GetTeam(const Player& player) const
{
	auto i = m_teams.find(const_cast<Player*>(&player));
	AssertThrow("Game::GetTeam: Team not found: " + player.GetName(), i != m_teams.end());
	const Team* pTeam = i->second.get();
	AssertThrow("Game::GetTeam: Team not chosen: " + player.GetName(), !!pTeam);
	return *pTeam;
}

Team& Game::GetTeam(Player& player)
{
	return const_cast<Team&>(const_cast<const Game*>(this)->GetTeam(player));
}

Player& Game::GetCurrentPlayer() 
{
	return const_cast<Player&>(const_cast<const Game*>(this)->GetCurrentPlayer());
}

const Player& Game::GetCurrentPlayer() const
{
	AssertStarted();

	return *m_teamOrder[(m_iStartTeam + m_iTurn) % m_teams.size()];
}

Team& Game::GetCurrentTeam()
{
	return GetTeam(GetCurrentPlayer());
}

const Team* Game::GetTeamFromColour(Colour c) const
{
	for (auto& i : m_teams)
		if (const Team* pTeam = i.second.get())
			if (pTeam->GetColour() == c)
				return pTeam;
	return nullptr;
}

void Game::AssignTeam(Player& player, RaceType race, Colour colour)
{
	auto i = m_teams.find(&player);
	AssertThrow("Game::AssignTeam: Team not found: " + player.GetName(), i != m_teams.end());
	AssertThrow("Game::AssignTeam: Team already assigned: " + player.GetName(), i->second == nullptr);

	i->second = TeamPtr(new Team(*this, player, race, colour));

	AdvanceTurn();
	if (m_iTurn == 0)
		StartMainPhase();
}

void Game::AdvanceTurn()
{
	m_iTurn = (m_iTurn + 1) % m_teams.size();
}

