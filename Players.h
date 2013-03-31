#pragma once

#include "Player.h"

#include <map>
#include <memory>

class Players
{
public:
	Players();

	Player& AddPlayer(const std::string& name, const std::string& password);

	Player& FindPlayer(int idPlayer);
	const Player& FindPlayer(int idPlayer) const;
	const Player* FindPlayer(const std::string& name) const;

private:
	std::map<int, PlayerPtr> m_map;
	int m_nNextID;
};

