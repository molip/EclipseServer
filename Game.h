#pragma once

#include "Map.h"
#include "Team.h"
#include "Player.h"

#include <memory>
#include <set>
#include <map>

class Game
{
public:
	Game(const std::string& name, const std::string& owner);
	const std::string& GetName() const { return m_name; }
	const std::string& GetOwner() const { return m_owner; }

	bool AddPlayer(const std::string& name);
	const std::set<std::string>& GetPlayers() const { return m_players; }

//	const std::map<std::string, PlayerPtr>& GetPlayers() const { return m_players; }

	void Start() { m_started = true; }
	bool HasStarted() const { return m_started; }

private:
	std::string m_name, m_owner;
	bool m_started;
	Map	m_map;

	std::set<TeamPtr> m_teams;
	std::set<std::string> m_players;
};

typedef std::unique_ptr<Game> GamePtr;
