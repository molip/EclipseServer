#include "stdafx.h"
#include "Game.h"

Game::Game(const std::string& name, const std::string& owner) : m_name(name), m_owner(owner), m_started(false)
{
}

bool Game::AddPlayer(const std::string& name)
{
	return m_players.insert(name).second;
}
