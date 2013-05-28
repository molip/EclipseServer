#pragma once

#include "Game.h"

class Controller;

class Games
{
public:
	static Game& Add(const std::string& name, Player& owner);
	static const std::vector<GamePtr>& GetGames() { return s_games; }
	static Game& Get(int idGame);

private:
	static std::vector<GamePtr> s_games;
	static int s_nNextGameID;
};