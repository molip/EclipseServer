#include "stdafx.h"
#include "Game.h"

Game::Game(const std::string& name, const std::string& owner) : m_name(name), m_owner(owner)
{
}

bool Game::AddPlayer(const std::string& name)
{
	return m_players.insert(std::make_pair(name, PlayerPtr(new Player(name)))).second;
}

Player* Game::FindPlayer(const std::string& name)
{
	auto i = m_players.find(name);
	if (i != m_players.end())
		return i->second.get();
	return nullptr;
}

bool Game::DeletePlayer(const std::string& name)
{
	return m_players.erase(name) == 1;
}
