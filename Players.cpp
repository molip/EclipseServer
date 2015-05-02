#include "stdafx.h"
#include "Players.h"
#include "App.h"
#include "OS.h"
#include "Xml.h"
#include "Serial.h"
#include "Util.h"

int Players::s_nNextID = 1;
int Players::s_nNextTestID = -1;
std::map<int, PlayerPtr> Players::s_map;

Player& Players::Add(const std::string& email, const std::string& name, const std::string& password)
{
	VERIFY(Find(email) == nullptr);

	Player* p = new Player(s_nNextID, Util::ToLower(email), name, password);
	ASSERT(s_map.insert(std::make_pair(s_nNextID, PlayerPtr(p))).second);
	++s_nNextID;
	return *p;
}

Player& Players::AddTest()
{
	Player* p = new Player(s_nNextTestID, "", ::FormatString("Test %0", -s_nNextTestID), "");
	ASSERT(s_map.insert(std::make_pair(s_nNextTestID, PlayerPtr(p))).second);
	--s_nNextTestID;
	return *p;
}

Player& Players::Get(int idPlayer) 
{
	auto i = s_map.find(idPlayer);
	VERIFY_MODEL(i != s_map.end());
	return *i->second;
}

Player* Players::Find(int idPlayer)
{
	auto i = s_map.find(idPlayer);
	return i == s_map.end() ? nullptr : i->second.get();
}

Player* Players::Find(const std::string& email) 
{
	std::string lower = Util::ToLower(email);

	for (auto& i : s_map)
		if (lower == i.second->GetEmail())
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
