#include "stdafx.h"
#include "ShipLayout.h"
#include "Ship.h"
#include "EnumTraits.h"

ShipLayout::ShipLayout(ShipType type) : m_type(type)
{
	int slots = 0;
	switch (type)
	{
	case ShipType::Interceptor:	slots = 4; break;
	case ShipType::Starbase:	slots = 5; break;
	case ShipType::Cruiser:		slots = 6; break;
	case ShipType::Dreadnought:	slots = 8; break;
	case ShipType::Ancient:		slots = 3; break;
	case ShipType::GCDS:		slots = 11; break;
	}
	m_slots.resize(slots);
}

DEFINE_ENUM_NAMES(ShipPart) {	"Empty", "Blocked", 
								"IonCannon", "PlasmaCannon", "AntimatterCannon", 
								"PlasmaMissile", 
								"ElectronComp", "GluonComp", "PositronComp", 
								"NuclearDrive", "TachyonDrive", "FusionDrive",
								"NuclearSource", "TachyonSource", "FusionSource", 
								"PhaseShield", "GaussShield", 
								"Hull", "ImprovedHull", "" };
