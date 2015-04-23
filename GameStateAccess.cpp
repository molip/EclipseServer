#include "GameStateAccess.h"

#include "Game.h"

GameState& GameStateAccess::GetGameState(Game& game)
{ 
	return game.m_state; 
}
