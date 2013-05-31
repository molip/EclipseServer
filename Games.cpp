#include "Games.h"
#include "LiveGame.h"

int Games::s_nNextGameID = 1;
std::vector<LiveGamePtr> Games::s_liveGames;

LiveGame& Games::Add(const std::string& name, Player& owner)
{
	s_liveGames.push_back(LiveGamePtr(new LiveGame(s_nNextGameID++, name, owner)));
	return *s_liveGames.back().get();
}

LiveGame& Games::GetLive(int idGame)
{
	LiveGame* pGame = nullptr;
	for (auto& g : s_liveGames)
		if (g->GetID() == idGame)
		{
			pGame = g.get();
			break;
		}
	AssertThrow("Games::GetLive", !!pGame);
	return *pGame;
}

Game& Games::Get(int idGame)
{
	return GetLive(idGame);
}