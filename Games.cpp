#include "Games.h"
#include "LiveGame.h"
#include "ReviewGame.h"

int Games::s_nNextGameID = 1;
std::vector<LiveGamePtr> Games::s_liveGames;
std::vector<ReviewGamePtr> Games::s_reviewGames;

LiveGame& Games::Add(const std::string& name, Player& owner)
{
	s_liveGames.push_back(LiveGamePtr(new LiveGame(s_nNextGameID++, name, owner)));
	return *s_liveGames.back().get();
}

ReviewGame& Games::AddReview(Player& owner, const LiveGame& live)
{
	s_reviewGames.push_back(ReviewGamePtr(new ReviewGame(s_nNextGameID++, owner, live)));
	return *s_reviewGames.back().get();
}

void Games::DeleteReview(int idGame)
{
	for (auto i = s_reviewGames.begin(); i != s_reviewGames.end(); ++i)
		if ((*i)->GetID() == idGame)
		{
			s_reviewGames.erase(i);
			return;
		}
	AssertThrow("Games::DeleteReview", false);
}

LiveGame& Games::GetLive(int idGame)
{
	for (auto& g : s_liveGames)
		if (g->GetID() == idGame)
			return *g;

	AssertThrow("Games::GetLive", false);
	return *(LiveGame*)nullptr;
}

ReviewGame& Games::GetReview(int idGame)
{
	for (auto& g : s_reviewGames)
		if (g->GetID() == idGame)
			return *g;

	AssertThrow("Games::GetReview", false);
	return *(ReviewGame*)nullptr;
}

Game& Games::Get(int idGame)
{
	for (auto& g : s_liveGames)
		if (g->GetID() == idGame)
			return *g;
	for (auto& g : s_reviewGames)
		if (g->GetID() == idGame)
			return *g;

	AssertThrow("Games::Get", false);
	return *(Game*)nullptr;
}