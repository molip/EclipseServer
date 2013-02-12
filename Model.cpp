#include "Model.h"

void Model::AddGame(const std::string& name, const std::string& owner)
{
	m_games.push_back(GamePtr(new Game(name, owner)));
}

Game* Model::FindGame(const std::string& game)
{
	for (auto& g : m_games)
		if (g->GetName() == game)
			return g.get();
	return nullptr;
}
