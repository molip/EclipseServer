#pragma once

#include "App.h"

class LiveGame;
class ReviewGame;
class Game;
class Player;

DEFINE_UNIQUE_PTR(LiveGame)
DEFINE_UNIQUE_PTR(ReviewGame)

class Controller;

class Games
{
public:
	static void Load();
	static LiveGame& Add(const std::string& name, Player& owner);
	static ReviewGame& AddReview(Player& owner, const LiveGame& live);
	static void DeleteReview(int idGame);
	static const std::vector<LiveGamePtr>& GetLiveGames() { return s_liveGames; }
	static const std::vector<ReviewGamePtr>& GetReviewGames() { return s_reviewGames; }
	static Game& Get(int idGame);
	static LiveGame& GetLive(int idGame);
	static ReviewGame& GetReview(int idGame);

private:
	static std::vector<LiveGamePtr> s_liveGames;
	static std::vector<ReviewGamePtr> s_reviewGames;
	static int s_nNextGameID;
};