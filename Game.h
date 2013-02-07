#pragma once

#include "Map.h"
#include "Team.h"
#include "Player.h"

#include <memory>
#include <set>

class Game
{
public:
	Game(const std::string& name, const std::string& owner);
	const std::string& GetName() const { return m_name; }
	const std::string& GetOwner() const { return m_owner; }

	bool AddPlayer(const std::string& name);
	Player* FindPlayer(const std::string& name);
	bool DeletePlayer(const std::string& name);
private:
	std::string m_name, m_owner;

	Map	m_map;

	std::set<TeamPtr> m_teams;
	std::set<PlayerPtr> m_players;
};

typedef std::unique_ptr<Game> GamePtr;
