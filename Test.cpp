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
			int count = 0;
			switch (type)
			{
			case ShipType::Interceptor: count = 8; break;
			//case ShipType::Cruiser: count = 4; break;
			//case ShipType::Dreadnought: count = 2; break;
			//case ShipType::Starbase: count = 4; break;
			}

			for (int i = 0; i < count; ++i)
				hex->AddShip(type, team->GetColour());
		}
	}
}
