#include "Discovery.h"

DiscoveryClass Discovery::GetClass(DiscoveryType d)
{
	switch (d)
	{
	case DiscoveryType::None:			return DiscoveryClass::None; 
	case DiscoveryType::AncientTech:	return DiscoveryClass::Tech; 
	case DiscoveryType::AncientCruiser:	return DiscoveryClass::Ship;
	}
	return int(d) < int(DiscoveryType::AxiomComputer) ? DiscoveryClass::Resource : DiscoveryClass::ShipPart;
}
