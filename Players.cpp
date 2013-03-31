#include "stdafx.h"
#include "Players.h"
#include "App.h"

Players::Players() : m_nNextID(1)
{
	AddPlayer("jon", "noj");	// 1
	AddPlayer("alex", "xela");	// 2
	AddPlayer("ben", "neb");	// 3
}

Player& Players::AddPlayer(const std::string& name, const std::string& password)
{
	Player* p = new Player(m_nNextID, name, password);
	ASSERT(m_map.insert(std::make_pair(m_nNextID, PlayerPtr(p))).second);
	++m_nNextID;
	return *p;
}

Player& Players::FindPlayer(int idPlayer)
{
	return const_cast<Player&>(const_cast<const Players*>(this)->FindPlayer(idPlayer));
}

const Player& Players::FindPlayer(int idPlayer) const
{
	auto i = m_map.find(idPlayer);
	AssertThrow("Players::FindPlayer", i != m_map.end());
	return *i->second;
}

const Player* Players::FindPlayer(const std::string& name) const
{
	for (auto& i : m_map)
		if (name == i.second->GetName())
			return i.second.get();
	return nullptr;
}