#pragma once

#include "App.h"

class LiveGame;
class Game;
class Player;

DEFINE_UNIQUE_PTR(LiveGame)

class Controller;

class Games
{
public:
	static LiveGame& Add(const std::string& name, Player& owner);
	static const std::vector<LiveGamePtr>& GetLiveGames() { return s_liveGames; }
	static Game& Get(int idGame);
	static LiveGame& GetLive(int idGame);

private:
	static std::vector<LiveGamePtr> s_liveGames;
	static int s_nNextGameID;
};