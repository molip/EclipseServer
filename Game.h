#pragma once

#include "Team.h"
#include "Bag.h"
#include "Cmd.h"
#include "GameState.h"
#include "Types.h"

#include <memory>
#include <set>
#include <string>
#include <deque>
#include <list>

class CmdStack;
class Record;

namespace Serial { class SaveNode; class LoadNode; }

class Game
{
	friend class GameStateAccess;
public:
	Game();
	Game(int id, const std::string& name, const Player& owner);
	Game(int id, const std::string& name, const Player& owner, const Game& rhs);
	virtual ~Game();
	
	int GetID() const { return m_id; }
	const std::string& GetName() const { return m_name; }
	const Player& GetOwner() const;

	const std::vector<TeamPtr>& GetTeams() const { return m_teams; }

	Team* FindTeam(const Player& player);

	Team* FindTeam(Colour c);
	Team& GetTeam(Colour c);
	Team& GetTeam(const Player& player);

	const Team* FindTeam(Colour c) const			{ return const_cast<Game*>(this)->FindTeam(c); }
	const Team* FindTeam(const Player& player) const{ return const_cast<Game*>(this)->FindTeam(player); }
	const Team& GetTeam(Colour c) const				{ return const_cast<Game*>(this)->GetTeam(c); }
	const Team& GetTeam(const Player& player) const	{ return const_cast<Game*>(this)->GetTeam(player); }

	virtual bool HasStarted() const = 0;
	virtual bool IsLive() const { return false; }
	virtual std::vector<std::pair<int, std::string>> GetLogs() const = 0;

	const ReputationBag& GetReputationBag() const { return const_cast<GameState&>(m_state).GetReputationBag(); }
	const TechnologyBag& GetTechnologyBag() const { return m_techBag; }
	const DiscoveryBag& GetDiscoveryBag() const { return m_discBag; }
	const HexBag& GetHexBag(HexRing ring) const { return m_hexBag[(int)ring]; }

	bool IsHexBagEmpty(HexRing ring) const { return const_cast<GameState&>(m_state).GetHexBag(ring).IsEmpty(); }

	const Map& GetMap() const { return m_state.m_map; }

	const std::map<TechType, int>& GetTechnologies() const { return m_state.m_techs; }

	int GetRound() const { return m_state.m_iRound; }
	bool HasFinished() const;

	virtual void Save(Serial::SaveNode& node) const;
	virtual void Load(const Serial::LoadNode& node);

	std::set<const Player*> GetCurrentPlayers() const { return m_players; }
	void AddPlayer(const Player* player) const { m_players.insert(player); }
	void RemovePlayer(const Player* player) const { m_players.erase(player); }

	const ShipBattle& GetShipBattle() const { return const_cast<GameState&>(m_state).GetShipBattle(); }
	const PopulationBattle& GetPopulationBattle() const { return const_cast<GameState&>(m_state).GetPopulationBattle(); }
	const Battle& GetBattle() const { return const_cast<GameState&>(m_state).GetBattle(); }
	bool HasBattle() const { return !!m_state.m_battle; }

	static const int RoundCount;

protected:
	std::vector<TeamPtr> m_teams;
	mutable std::set<const Player*> m_players; // Not saved. Includes observers.

	// Immutable
	int m_id; 
	std::string m_name;
	int m_idOwner;
	TechnologyBag m_techBag;
	DiscoveryBag m_discBag;
	HexBag m_hexBag[(int)HexRing::_Count];

	GameState m_state;
};

typedef std::unique_ptr<Game> GamePtr;

