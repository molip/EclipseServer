#pragma once

#include "Resources.h"

enum class ShipPart;

enum class DiscoveryType {	
	None = -1, Money, Science, Materials, AncientTech, AncientCruiser, 
	AxionComputer, HypergridSource, ShardHull, IonTurret, ConformalDrive, FluxShield, 
};

enum class DiscoveryClass { None = -1, Resource, Tech, Ship, ShipPart };

class Discovery
{
public:
	static DiscoveryClass GetClass(DiscoveryType d);
	static Storage GetResources(DiscoveryType d);
	static Resource GetResourceType(DiscoveryType d);
	static ShipPart GetShipPart(DiscoveryType d);
};
