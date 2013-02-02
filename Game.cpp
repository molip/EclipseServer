#include "stdafx.h"
#include "Game.h"

Game::Game(const std::string& name, const std::string& owner) : m_name(name), m_owner(owner)
{
}

bool Game::AddPlayer(const std::string& name)
{
	return m_players.insert(PlayerPtr(new Player(name))).second;
}

Player* Game::FindPlayer(const std::string& name)
{
	for (auto& p : m_players)
		if (p->GetName() == name)
			return p.get();
	return nullptr;
}

bool Game::DeletePlayer(const std::string& name)
{
	for (auto i = m_players.begin(); i != m_players.end(); ++i)
		if ((*i)->GetName() == name)
		{
			m_players.erase(i);
			return true;
		}
	return false;
}
