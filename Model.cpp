#include "Model.h"

Game& Model::AddGame(const std::string& name, Player& owner)
{
	m_games.push_back(GamePtr(new Game(name, owner)));
	return *m_games.back().get();
}

Game* Model::FindGame(const std::string& game)
{
	for (auto& g : m_games)
		if (g->GetName() == game)
			return g.get();
	return nullptr;
}
