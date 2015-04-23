#pragma once

#include "GameStateAccess.h"

class LiveGame;

class Test : public GameStateAccess
{
public:
	static void Run();
	static void AddShipsToCentre(LiveGame& game);
};
