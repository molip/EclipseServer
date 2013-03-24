#pragma once

#include "Map.h"
#include "Team.h"
#include "Bag.h"

#include <memory>
#include <set>
#include <map>
#include <string>

enum class HexRing { Inner, Middle, Outer, _Count };

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

	void AssignTeam(const std::string& name, RaceType race, Colour colour);
	bool HasTeamChosen(const std::string& name) const;
	const Team& GetTeam(const std::string& name) const;
	const std::string& GetCurrentTeamName() const;
	Team& GetCurrentTeam();

	void StartChooseTeamPhase();
	void StartMainPhase();
	bool HasStarted() const { return m_phase != Phase::Lobby; }
	Phase GetPhase() const { return m_phase; }

	ReputationBag& GetReputationBag() { return m_repBag; }
	TechnologyBag& GetTechnologyBag() { return m_techBag; }
	DiscoveryBag& GetDiscoveryBag() { return m_discBag; }

private:
	Team& GetTeam(const std::string& name);
	void StartRound();

	void AssertStarted() const;
	void AdvanceTurn();
	
	std::string m_name, m_owner;
	Phase m_phase;

	Map	m_map;
	std::multiset<TechType> m_techs;

	std::map<std::string, TeamPtr> m_teams;
	ReputationBag m_repBag;
	TechnologyBag m_techBag;
	DiscoveryBag m_discBag;
	HexBag m_hexBag[HexRing::_Count];

	std::vector<std::string> m_teamOrder;
	int m_iTurn, m_iRound;
	int m_iStartTeam, m_iStartTeamNext;
};

typedef std::unique_ptr<Game> GamePtr;
