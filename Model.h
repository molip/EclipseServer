#pragma once

#include "Game.h"

class Controller;

class Model
{
public:
	Model() : m_pController(nullptr) {}

	void SetController(Controller* p) { m_pController = p; }

	void AddGame(const std::string& name, const std::string& owner);

	const std::vector<GamePtr>& GetGames() const { return m_games; }

private:
	std::vector<GamePtr> m_games;

	Controller* m_pController;
};