#include "Model.h"

Model::Model() : m_pController(nullptr), m_nNextGameID(1)
{
}

Game& Model::AddGame(const std::string& name, Player& owner)
{
	m_games.push_back(GamePtr(new Game(m_nNextGameID++, name, owner)));
	return *m_games.back().get();
}

Game* Model::FindGame(int idGame)
{
	for (auto& g : m_games)
		if (g->GetID() == idGame)
			return g.get();
	return nullptr;
}
