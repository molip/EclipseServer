#include "stdafx.h"
#include "Game.h"
#include "App.h"

#include <algorithm>

Game::Game(const std::string& name, const std::string& owner) : 
	m_name(name), m_owner(owner), m_phase(Phase::Lobby), m_iTurn(-1), m_iRound(-1), m_iStartTeam(-1), m_iStartTeamNext(-1)
{
}

bool Game::AddTeam(const std::string& name)
{
	AssertThrow("Game::AddTeam: Game already started: " + m_name, !HasStarted());
	return m_teams.insert(std::make_pair(name, nullptr)).second; // OK if already added.
}

void Game::Start()
{
	AssertThrow("Game::Start: Game already started: " + m_name, !HasStarted());
	AssertThrow("Game::Start: Game has no players: " + m_name, !m_teams.empty());

	m_iTurn = m_iRound = m_iStartTeam = m_iStartTeamNext = 0;

	for (auto i = m_teams.begin(); i != m_teams.end(); ++i)
		m_teamOrder.push_back(i->first);

	std::random_shuffle(m_teamOrder.begin(), m_teamOrder.end());

	m_phase = Phase::ChooseTeam;
}

void Game::AssertStarted() const
{
	AssertThrow("Game not started yet: " + m_name, HasStarted());
}

const Team* Game::GetTeam(const std::string& name) const
{
	auto i = m_teams.find(name);
	AssertThrow("Game::GetTeam: Team not found: " + name, i != m_teams.end());
	return i->second.get();
}

const std::string& Game::GetCurrentTeamName() const
{
	AssertStarted();

	return m_teamOrder[(m_iStartTeam + m_iTurn) % m_teams.size()];
}

Team* Game::GetCurrentTeam()
{
	auto i = m_teams.find(GetCurrentTeamName());
	return i->second.get();
}

void Game::AssignTeam(const std::string& name, Race race, Colour colour)
{
	auto i = m_teams.find(name);
	AssertThrow("Game::AssignTeam: Team not found: " + name, i != m_teams.end());
	AssertThrow("Game::AssignTeam: Team already assigned: " + name, i->second == nullptr);

	i->second = TeamPtr(new Team(name, race, colour));

	AdvanceTurn();
	if (m_iTurn == 0)
		m_phase = Phase::Main;
}

void Game::AdvanceTurn()
{
	m_iTurn = (m_iTurn + 1) % m_teams.size();
}

