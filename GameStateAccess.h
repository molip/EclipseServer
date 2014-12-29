#pragma once

class Game;
class GameState;

class GameStateAccess
{
public:
	GameState& GetGameState(Game& game) const;
};
