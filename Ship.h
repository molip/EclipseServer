#pragma once

//#include "Blueprint.h"
//#include "ShipLayout.h"

enum class ShipType { None = -3, _First = -2, Ancient = -2, GCDS = -1, Interceptor, Cruiser, Dreadnought, Starbase, _Count };

extern int GetShipTypeSize(ShipType shipType);
