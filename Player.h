#pragma once

#include <string>
#include <memory>

class Game;
class LiveGame;
class ReviewGame;
class Team;

class Player
{
public:
	Player();
	Player(int id, const std::string& name, const std::string& password);

	int GetID() const { return m_id; }
	const std::string& GetName() const { return m_name; }
	const std::string& GetPassword() const { return m_password; }

	const LiveGame* GetCurrentLiveGame() const;
	const ReviewGame* GetCurrentReviewGame() const;
	const Game* GetCurrentGame() const;
	void SetCurrentGame(const Game* pGame);

	const Team* GetCurrentTeam() const;
	//const Team* GetCurrentTeam() const { return const_cast<Player*>(this)->GetCurrentTeam(); }

private:
	int m_id;
	int m_idCurrentGame;
	std::string m_name, m_password;
};

typedef std::unique_ptr<Player> PlayerPtr;