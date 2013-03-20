#include "stdafx.h"
#include "ShipLayout.h"
#include "Ship.h"

ShipLayout::ShipLayout(ShipType type) : m_type(type)
{
	int slots = 0;
	switch (type)
	{
	case ShipType::Interceptor:	slots = 4; break;
	case ShipType::Starbase:	slots = 5; break;
	case ShipType::Cruiser:		slots = 6; break;
	case ShipType::Dreadnought:	slots = 8; break;
	}
	m_slots.resize(slots);
}

