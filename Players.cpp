#include "stdafx.h"
#include "Players.h"
#include "App.h"
#include "OS.h"
#include "Xml.h"
#include "Serial.h"

int Players::s_nNextID = 1;
std::map<int, PlayerPtr> Players::s_map;

Player& Players::Add(const std::string& name, const std::string& password)
{
	Player* p = new Player(s_nNextID, name, password);
	ASSERT(s_map.insert(std::make_pair(s_nNextID, PlayerPtr(p))).second);
	++s_nNextID;
	return *p;
}

Player& Players::Get(int idPlayer) 
{
	auto i = s_map.find(idPlayer);
	VERIFY_MODEL(i != s_map.end());
	return *i->second;
}

Player* Players::Find(const std::string& name) 
{
	for (auto& i : s_map)
		if (name == i.second->GetName())
			return i.second.get();
	return nullptr;
}

void Players::RejoinCurrentGame()
{
	for (auto& i : s_map)
		i.second->RejoinCurrentGame();
}

std::string Players::GetPath()
{
	return "data/players/";
}

void Players::Load()
{
	VERIFY_SERIAL(s_map.empty());

	auto files = OS::FindFilesInDir(GetPath(), "*.xml");

	for (auto& f : files)
	{
		PlayerPtr player(new Player);
		if (Serial::LoadClass(GetPath() + f, *player))
		{
			s_nNextID = std::max(s_nNextID, player->GetID() + 1);
			ASSERT(s_map.insert(std::make_pair(player->GetID(), std::move(player))).second);
		}
		else
			ASSERT(false);
	}
}
