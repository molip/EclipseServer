#pragma once

#include "Game.h"

class Controller;

class Model
{
public:
	Model() : m_pController(nullptr) {}

	void SetController(Controller* p) { m_pController = p; }

	Game& AddGame(const std::string& name, const std::string& owner);

	const std::vector<GamePtr>& GetGames() const { return m_games; }

	Game* FindGame(const std::string& game);

private:
	std::vector<GamePtr> m_games;

	Controller* m_pController;
};