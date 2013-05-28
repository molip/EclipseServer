#pragma once

#include "Player.h"

#include <map>
#include <memory>

class Players
{
public:
	static Player& Add(const std::string& name, const std::string& password);

	static Player& Get(int idPlayer);
	static Player* Find(const std::string& name);

private:
	static std::map<int, PlayerPtr> s_map;
	static int s_nNextID;
};

