#pragma once

#include <string>
#include <memory>
#include <map>

class Game;

class Player
{
public:
	Player(const std::string& name);
	~Player();

	const std::string& GetName() const { return m_name; }

	Game* GetCurrentGame() const { return m_pCurrentGame; }
	void SetCurrentGame(Game* pGame) { m_pCurrentGame = pGame; }

private:
	std::string m_name;
	Game* m_pCurrentGame;
};

typedef std::unique_ptr<Player> PlayerPtr;

class Players
{
public:
	bool AddPlayer(const std::string& name);
	Player* FindPlayer(const std::string& name);
	const Player* FindPlayer(const std::string& name) const;
	bool DeletePlayer(const std::string& name);

private:
	std::map<std::string, PlayerPtr> m_map;
};