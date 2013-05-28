#pragma once

enum class DiscoveryType {	
	None = -1, Money, Science, Materials, AncientTech, AncientCruiser, 
	AxiomComputer, HypergridSource, ShardHull, IonTurret, ConformalDrive, FluxShield, 
};

enum class DiscoveryClass { None = -1, Resource, Tech, Ship, ShipPart };

class Discovery
{
public:
	static DiscoveryClass GetClass(DiscoveryType d);
};



