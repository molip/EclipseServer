#include "Test.h"
#include "App.h"
#include "Hex.h"
#include "Map.h"
#include "LiveGame.h"

void Test::AddShipsToCentre(LiveGame& game)
{
	Hex* hex = const_cast<Hex*>(game.GetMap().FindHex(1));
	for (auto& team : game.GetTeams())
	{
		for (auto type : EnumRange<ShipType>())
		{
			int count;
			switch (type)
			{
			case ShipType::Interceptor: count = 8;
			case ShipType::Cruiser: count = 4;
			case ShipType::Dreadnought: count = 2;
			case ShipType::Starbase: count = 4;
			}

			for (int i = 0; i < count; ++i)
				hex->AddShip(type, team->GetColour());
		}

		game.ShipMovedTo(*hex, team->GetColour());
	}
}
