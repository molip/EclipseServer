#include "Discovery.h"
#include "EnumTraits.h"

DiscoveryClass Discovery::GetClass(DiscoveryType d)
{
	switch (d)
	{
	case DiscoveryType::None:			return DiscoveryClass::None; 
	case DiscoveryType::AncientTech:	return DiscoveryClass::Tech; 
	case DiscoveryType::AncientCruiser:	return DiscoveryClass::Ship;
	}
	return int(d) < int(DiscoveryType::AxionComputer) ? DiscoveryClass::Resource : DiscoveryClass::ShipPart;
}

DEFINE_ENUM_NAMES2(DiscoveryType, -1) {	
	"None", "Money", "Science", "Materials", "Ancient tech", "Ancient cruiser", 
	"Axion computer", "Hypergrid source", "Shard hull", "Ion turret", "Conformal drive", "Flux shield", "" };

DEFINE_ENUM_NAMES2(DiscoveryClass, -1) { "None", "Resource", "Tech", "Ship", "ShipPart", "" };