#include "GameStateAccess.h"

#include "Game.h"

GameState& GameStateAccess::GetGameState(Game& game) const
{ 
	return game.m_state; 
}
