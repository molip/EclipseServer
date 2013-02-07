#include "Model.h"

void Model::AddGame(const std::string& name, const std::string& owner)
{
	m_games.push_back(GamePtr(new Game(name, owner)));
}
