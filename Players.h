#pragma once

#include "Player.h"

#include <map>
#include <memory>

class Players
{
public:
	static void Load();
	static std::wstring GetPath();

	static Player& Add(const std::string& email, const std::string& name, const std::string& password);
	static Player& AddTest();

	static Player& Get(int idPlayer);
	static Player* Find(int idPlayer);
	static Player* Find(const std::string& email);

	static void RejoinCurrentGame();

private:
	static std::map<int, PlayerPtr> s_map;
	static int s_nNextID, s_nNextTestID;
};

