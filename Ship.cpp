#include "stdafx.h"
#include "Ship.h"
#include "EnumTraits.h"

DEFINE_ENUM_NAMES2(ShipType, -3) { "None", "Ancient", "GCDS", "Interceptor", "Cruiser", "Dreadnought", "Starbase", "" };

int GetShipTypeSize(ShipType shipType)
{
	switch (shipType)
	{
	case ShipType::Interceptor:	return 4;
	case ShipType::Cruiser:		return 6;
	case ShipType::Dreadnought:	return 8;
	case ShipType::Starbase:	return 5;
	}

	return 0;
}

