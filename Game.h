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

	Game(const std::string& name, Player& owner);
	const std::string& GetName() const { return m_name; }
	const Player& GetOwner() const { return m_owner; }

	bool AddTeam(Player& player);
	const std::map<Player*, TeamPtr>& GetTeams() const { return m_teams; }
	const std::vector<Player*>& GetTeamOrder() const { return m_teamOrder; }

	void AssignTeam(Player& player, RaceType race, Colour colour);
	bool HasTeamChosen(Player& player) const;
	const Team& GetTeam(const Player& player) const;
	const Player& GetCurrentPlayer() const;
	Player& GetCurrentPlayer();
	Team& GetCurrentTeam();
	const Team* GetTeamFromColour(Colour c) const;

	void StartChooseTeamPhase();
	void StartMainPhase();
	bool HasStarted() const { return m_phase != Phase::Lobby; }
	Phase GetPhase() const { return m_phase; }

	ReputationBag& GetReputationBag() { return m_repBag; }
	TechnologyBag& GetTechnologyBag() { return m_techBag; }
	DiscoveryBag& GetDiscoveryBag() { return m_discBag; }

	const Map& GetMap() const { return m_map; }
	Map& GetMap() { return m_map; }

private:
	Team& GetTeam(Player& player);
	void StartRound();

	void AssertStarted() const;
	void AdvanceTurn();
	
	std::string m_name;
	Player& m_owner;
	Phase m_phase;

	Map	m_map;
	std::multiset<TechType> m_techs;

	std::map<Player*, TeamPtr> m_teams;
	
	ReputationBag m_repBag;
	TechnologyBag m_techBag;
	DiscoveryBag m_discBag;
	HexBag m_hexBag[HexRing::_Count];

	std::vector<Player*> m_teamOrder;
	std::set<RaceType, Colour> m_availableRaces;
	int m_iTurn, m_iRound;
	int m_iStartTeam, m_iStartTeamNext;
};

typedef std::unique_ptr<Game> GamePtr;
