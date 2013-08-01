#pragma once

#include "Map.h"
#include "Team.h"
#include "Bag.h"
#include "Cmd.h"

#include <memory>
#include <set>
#include <map>
#include <string>
#include <deque>
#include <list>

enum class HexRing { Inner, Middle, Outer, _Count };

class CmdStack;
class Record;

namespace Serial { class SaveNode; class LoadNode; }

class Game
{
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
	const Team& GetTeam(Colour c) const				{ return const_cast<Game*>(this)->GetTeam(c); }
	const Team& GetTeam(const Player& player) const	{ return const_cast<Game*>(this)->GetTeam(player); }

	virtual bool HasStarted() const = 0;
	virtual bool IsLive() const { return false; }

	ReputationBag& GetReputationBag() { return m_repBag; }
	TechnologyBag& GetTechnologyBag() { return m_techBag; }
	DiscoveryBag& GetDiscoveryBag() { return m_discBag; }
	HexBag& GetHexBag(HexRing ring) { return m_hexBag[(int)ring]; }
	const HexBag& GetHexBag(HexRing ring) const { return m_hexBag[(int)ring]; }

	const Map& GetMap() const { return m_map; }
	Map& GetMap() { return m_map; }

	const std::map<TechType, int>& GetTechnologies() const { return m_techs; }
	std::map<TechType, int>& GetTechnologies() { return m_techs; }

	virtual void Save(Serial::SaveNode& node) const;
	virtual void Load(const Serial::LoadNode& node);

protected:
	virtual void Save() const {}
	
	int m_id; 
	std::string m_name;
	int m_idOwner;

	std::map<TechType, int> m_techs;

	std::vector<TeamPtr> m_teams;
	
	ReputationBag m_repBag;
	TechnologyBag m_techBag;
	DiscoveryBag m_discBag;
	HexBag m_hexBag[HexRing::_Count];

	Map	m_map; // After m_discBag
};

typedef std::unique_ptr<Game> GamePtr;
