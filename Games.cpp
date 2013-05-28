#include "Games.h"

int Games::s_nNextGameID = 1;
std::vector<GamePtr> Games::s_games;

Game& Games::Add(const std::string& name, Player& owner)
{
	s_games.push_back(GamePtr(new Game(s_nNextGameID++, name, owner)));
	return *s_games.back().get();
}

Game& Games::Get(int idGame)
{
	Game* pGame = nullptr;
	for (auto& g : s_games)
		if (g->GetID() == idGame)
		{
			pGame = g.get();
			break;
		}
	AssertThrow("Games::Find", !!pGame);
	return *pGame;
}
