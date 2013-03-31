#pragma once

#include <string>
#include <memory>

class Game;

class Player
{
public:
	Player();
	Player(int id, const std::string& name, const std::string& password);

	int GetID() const { return m_id; }
	const std::string& GetName() const { return m_name; }
	const std::string& GetPassword() const { return m_password; }

	const Game* GetCurrentGame() const { return m_pCurrentGame; }
	Game* GetCurrentGame() { return m_pCurrentGame; }
	void SetCurrentGame(Game* pGame);

private:
	Game* m_pCurrentGame;
	int m_id;
	std::string m_name, m_password;
};

typedef std::unique_ptr<Player> PlayerPtr;