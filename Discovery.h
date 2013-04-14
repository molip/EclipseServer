#pragma once

enum class DiscoveryType {	
	None, Money, Science, Materials, AncientTech, AncientCruiser, 
	AxiomComputer, HypergridSource, ShardHull, IonTurret, ConformalDrive, FluxShield, 
};

enum class DiscoveryClass { None, Resource, Tech, Ship, ShipPart };

class Discovery
{
public:
	static DiscoveryClass GetClass(DiscoveryType d);
};



