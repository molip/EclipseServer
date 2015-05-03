#pragma once

#include <string>
#include <memory>

class Game;
class LiveGame;
class ReviewGame;
class Team;

namespace Serial { class SaveNode; class LoadNode; }

class Player
{
public:
	Player();
	Player(const Player&) = delete;
	Player(int id, const std::string& email, const std::string& name, const std::string& password);

	int GetID() const { return m_id; }
	const std::string& GetName() const { return m_name; }
	const std::string& GetEmail() const { return m_email; }
	bool CheckPasswordHash(const std::string& hash) const;

	const std::string& GetSessionHash() const { return m_sessionHash; }
	void SetSessionHash(const std::string& hash);

	const LiveGame* GetCurrentLiveGame() const;
	const ReviewGame* GetCurrentReviewGame() const;
	const Game* GetCurrentGame() const;
	const Team* GetCurrentTeam() const;
	bool CheckPassword(const std::string& password) const;

	void SetCurrentGame(const Game* pGame);
	void RejoinCurrentGame();

	void Save() const;
	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	std::string HashPassword(const std::string& password) const;
		
	int m_id;
	int m_idCurrentGame;
	std::string m_email, m_name, m_passwordHash;
	std::string m_sessionHash;
};

typedef std::unique_ptr<Player> PlayerPtr;