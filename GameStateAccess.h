#pragma once

class Game;
class GameState;

class GameStateAccess
{
public:
	static GameState& GetGameState(Game& game);
};
