#pragma once

#include "Map.h"
#include "Team.h"

#include <memory>
#include <set>
#include <map>
#include <string>

class Game
{
public:
	enum class Phase { Lobby, ChooseTeam, Main };

	Game(const std::string& name, const std::string& owner);
	const std::string& GetName() const { return m_name; }
	const std::string& GetOwner() const { return m_owner; }

	bool AddTeam(const std::string& name);
	const std::map<std::string, TeamPtr>& GetTeams() const { return m_teams; }
	const std::vector<std::string>& GetTeamOrder() const { return m_teamOrder; }

	void AssignTeam(const std::string& name, Race race, Colour colour);
	const Team* GetTeam(const std::string& name) const;
	const std::string& GetCurrentTeamName() const;
	Team* GetCurrentTeam();

	void Start();
	bool HasStarted() const { return m_phase != Phase::Lobby; }
	Phase GetPhase() const { return m_phase; }

private:
	void AssertStarted() const;
	void AdvanceTurn();
	
	std::string m_name, m_owner;
	Phase m_phase;

	Map	m_map;

	std::map<std::string, TeamPtr> m_teams;

	std::vector<std::string> m_teamOrder;
	int m_iTurn, m_iRound;
	int m_iStartTeam, m_iStartTeamNext;
};

typedef std::unique_ptr<Game> GamePtr;
