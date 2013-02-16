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

bool Players::DeletePlayer(const std::string& name)
{
	return m_map.erase(name) == 1;
}
