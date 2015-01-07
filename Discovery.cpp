#include "Discovery.h"
#include "EnumTraits.h"
#include "ShipLayout.h"

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

Storage Discovery::GetResources(DiscoveryType d)
{
	Storage r;
	switch (d)
	{
	case DiscoveryType::Materials:	r[Resource::Materials] = 6; break;
	case DiscoveryType::Money:		r[Resource::Money] = 8; break;
	case DiscoveryType::Science:	r[Resource::Science] = 5; break;
	default: VERIFY(false);
	}
	return r;
}

Resource Discovery::GetResourceType(DiscoveryType d)
{
	switch (d)
	{
	case DiscoveryType::Materials:	return Resource::Materials;
	case DiscoveryType::Money:		return Resource::Money;
	case DiscoveryType::Science:	return Resource::Science;
	}
	VERIFY(false);
	return Resource::None;
}

ShipPart Discovery::GetShipPart(DiscoveryType d)
{
	switch (d)
	{
	case DiscoveryType::AxionComputer:		return ShipPart::AxionComp;
	case DiscoveryType::ConformalDrive:		return ShipPart::ConformalDrive;
	case DiscoveryType::FluxShield:			return ShipPart::FluxShield;
	case DiscoveryType::HypergridSource:	return ShipPart::HypergridSource;
	case DiscoveryType::IonTurret:			return ShipPart::IonTurret;
	case DiscoveryType::ShardHull:			return ShipPart::ShardHull;
	default: VERIFY(false);
	}
	return ShipPart::Empty;
}

DEFINE_ENUM_NAMES2(DiscoveryType, -1) {	
	"None", "Money", "Science", "Materials", "Ancient tech", "Ancient cruiser", 
	"Axion computer", "Hypergrid source", "Shard hull", "Ion turret", "Conformal drive", "Flux shield", "" };

DEFINE_ENUM_NAMES2(DiscoveryClass, -1) { "None", "Resource", "Tech", "Ship", "ShipPart", "" };