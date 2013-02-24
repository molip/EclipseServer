#include "stdafx.h"
#include "Player.h"

Player::Player(const std::string& name) : m_name(name), m_pCurrentGame(nullptr)
{
}

Player::~Player()
{
}

//-----------------------------------------------------------------------------

bool Players::AddPlayer(const std::string& name)
{
	return m_map.insert(std::make_pair(name, PlayerPtr(new Player(name)))).second;
}

Player* Players::FindPlayer(const std::string& name)
{
	auto i = m_map.find(name);
	if (i != m_map.end())
		return i->second.get();
	return nullptr;
}

const Player* Players::FindPlayer(const std::string& name) const
{
	return const_cast<Players*>(this)->FindPlayer(name);
}

bool Players::DeletePlayer(const std::string& name)
{
	return m_map.erase(name) == 1;
}
