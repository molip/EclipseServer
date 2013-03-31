#pragma once

#include "Game.h"
#include "Players.h"

class Controller;

class Model
{
public:
	Model() : m_pController(nullptr) {}

	void SetController(Controller* p) { m_pController = p; }

	Game& AddGame(const std::string& name, Player& owner);

	const std::vector<GamePtr>& GetGames() const { return m_games; }

	Game* FindGame(const std::string& game);

	Player& FindPlayer(int idPlayer) { return m_players.FindPlayer(idPlayer); }
	const Player& FindPlayer(int idPlayer) const { return m_players.FindPlayer(idPlayer); }
	const Player* FindPlayer(const std::string& name) const  { return m_players.FindPlayer(name); }

private:
	std::vector<GamePtr> m_games;
	Players m_players;

	Controller* m_pController;
};