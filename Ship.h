#pragma once

//#include "Blueprint.h"
//#include "ShipLayout.h"

enum class ShipType { None = -3, Ancient = -2, GCDS = -1, Interceptor, Cruiser, Dreadnought, Starbase, _Count };

struct AllShipTypesRange : EnumRange<ShipType>
{
	AllShipTypesRange() : EnumRange<ShipType>(ShipType::Ancient) {}
};

struct PlayerShipTypesRange : EnumRange<ShipType>
{
	PlayerShipTypesRange() : EnumRange<ShipType>() {}
};

extern int GetShipTypeSize(ShipType shipType);
extern int GetShipTypeReputationTileCount(ShipType shipType);
