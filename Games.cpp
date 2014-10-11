#include "Games.h"
#include "LiveGame.h"
#include "ReviewGame.h"
#include "OS.h"
#include "Xml.h"
#include "Serial.h"

int Games::s_nNextGameID = 1;
std::vector<LiveGamePtr> Games::s_liveGames;
std::vector<ReviewGamePtr> Games::s_reviewGames;

void Games::Load()
{
	VERIFY_SERIAL(s_liveGames.empty() && s_reviewGames.empty());
	
	std::string dir = "data/games/live/";
	auto files = OS::FindFilesInDir(dir, "*.xml");
	
	for (auto& f : files)
	{
		LiveGamePtr pGame(new LiveGame);
		if (Serial::LoadClass(dir + f, *pGame))
		{
			s_nNextGameID = std::max(s_nNextGameID, pGame->GetID() + 1);
			s_liveGames.push_back(std::move(pGame));
		}
		else
			ASSERT(false);
	}
}

LiveGame& Games::Add(const std::string& name, Player& owner)
{
	s_liveGames.push_back(LiveGamePtr(new LiveGame(s_nNextGameID++, name, owner)));
	return *s_liveGames.back().get();
}

ReviewGame& Games::AddReview(Player& owner, const LiveGame& live)
{
	s_reviewGames.push_back(ReviewGamePtr(new ReviewGame(s_nNextGameID++, owner, live)));
	live.AddReviewGame(*s_reviewGames.back());
	return *s_reviewGames.back();
}

void Games::DeleteReview(int idGame)
{
	for (auto i = s_reviewGames.begin(); i != s_reviewGames.end(); ++i)
		if ((*i)->GetID() == idGame)
		{
			GetLive((*i)->GetLiveGameID()).RemoveReviewGame(**i);
			s_reviewGames.erase(i);
			return;
		}
	VERIFY_MODEL(false);
}

const LiveGame& Games::GetLive(int idGame)
{
	for (auto& g : s_liveGames)
		if (g->GetID() == idGame)
			return *g;

	VERIFY_MODEL(false);
	return *(LiveGame*)nullptr;
}

const ReviewGame& Games::GetReview(int idGame)
{
	for (auto& g : s_reviewGames)
		if (g->GetID() == idGame)
			return *g;

	VERIFY_MODEL(false);
	return *(ReviewGame*)nullptr;
}

const Game& Games::Get(int idGame)
{
	for (auto& g : s_liveGames)
		if (g->GetID() == idGame)
			return *g;
	for (auto& g : s_reviewGames)
		if (g->GetID() == idGame)
			return *g;

	VERIFY_MODEL(false);
	return *(Game*)nullptr;
}