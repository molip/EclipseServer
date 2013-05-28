#include "Race.h"
#include "App.h"
#include "Ship.h"
#include "Team.h"
#include "EnumTraits.h"

Storage Race::GetStartStorage() const
{
	switch (m_type)
	{
	case RaceType::Human:		return Storage(2, 3, 3);
	case RaceType::Eridani:		return Storage(26, 2, 4);
	case RaceType::Hydran:		return Storage(2, 5, 2);
	case RaceType::Planta:		return Storage(4, 4, 4);
	case RaceType::Descendants:	return Storage(2, 4, 3);
	case RaceType::Mechanema:	return Storage(3, 3, 3);
	case RaceType::Orion:		return Storage(3, 3, 5);
	}
	
	return Storage();
}

int Race::GetStartReputationTiles() const
{
	return m_type == RaceType::Eridani ? 2 : 0;
}

int Race::GetStartInfluenceDiscs() const
{
	return m_type == RaceType::Eridani ? 11 : 13;
}

int Race::GetStartColonyShips() const
{
	return m_type == RaceType::Planta ? 4 : 3;
}

ShipType Race::GetStartShip() const
{
	return m_type == RaceType::Orion ? ShipType::Cruiser : ShipType::Interceptor;
}

int Race::GetStartSector(Colour colour) const
{
	switch (m_type)
	{
	case RaceType::Eridani:		return 222;
	case RaceType::Hydran:		return 224;
	case RaceType::Planta:		return 226;
	case RaceType::Descendants:	return 228;
	case RaceType::Mechanema:	return 230;
	case RaceType::Orion:		return 232;
	}

	ASSERT(m_type == RaceType::Human);
	switch (colour)
	{
	case Colour::Red:		return 221;
	case Colour::Blue:		return 223;
	case Colour::Green:		return 225;
	case Colour::Yellow:	return 227;
	case Colour::White:		return 229;
	case Colour::Black:		return 231;
	}

	return 0;
}

int Race::GetMoves() const
{
	return m_type == RaceType::Human ? 3 : 2;
}

ReputationSlots Race::GetReputationSlots() const
{
	int a = 0, e = 4, r = 0;
	switch (m_type)
	{
	case RaceType::Hydran:		
	case RaceType::Planta:		e = 3;
	case RaceType::Human:		a = 1; break; 
	case RaceType::Orion:		r = 1;
	}

	return ReputationSlots(a, e, r);
}

BlueprintPtr Race::GetBlueprint(ShipType type) const
{
	ShipLayout layout(type);
	int fixedInitiative = 0, fixedPower = 0, fixedComputer = 0;

	layout.SetSlot(0, ShipPart::IonCannon);

	switch (type)
	{
	case ShipType::Interceptor:	
		layout.SetSlot(2, ShipPart::NuclearSource);
		layout.SetSlot(3, ShipPart::NuclearDrive);
		fixedInitiative = 2;
		break;
	case ShipType::Cruiser:
		layout.SetSlot(1, ShipPart::Hull);
		layout.SetSlot(2, ShipPart::NuclearSource);
		layout.SetSlot(3, ShipPart::ElectronComp);
		layout.SetSlot(5, ShipPart::NuclearDrive);
		fixedInitiative = 1;
		break;
	case ShipType::Dreadnought:
		layout.SetSlot(1, ShipPart::IonCannon);
		layout.SetSlot(2, ShipPart::Hull);
		layout.SetSlot(3, ShipPart::Hull);
		layout.SetSlot(4, ShipPart::ElectronComp);
		layout.SetSlot(6, ShipPart::NuclearSource);
		layout.SetSlot(7, ShipPart::NuclearDrive);
		break;
	case ShipType::Starbase:
		layout.SetSlot(1, ShipPart::Hull);
		layout.SetSlot(2, ShipPart::Hull);
		layout.SetSlot(3, ShipPart::ElectronComp);
		fixedInitiative = 4;
		fixedPower = 3;
		break;
	}

	switch (m_type)
	{
	case RaceType::Eridani:		
		if (type == ShipType::Dreadnought)
			fixedPower = 3;
		break;
	case RaceType::Planta:
		fixedComputer = 1;
		fixedPower += 2;
		switch (type)
		{
		case ShipType::Interceptor:	
			layout.SetSlot(1, ShipPart::Blocked);
			fixedInitiative = 0;
			break;
		case ShipType::Cruiser:
			layout.SetSlot(3, ShipPart::Empty);
			layout.SetSlot(4, ShipPart::Blocked);
			fixedInitiative = 0;
			break;
		case ShipType::Dreadnought:
			layout.SetSlot(3, ShipPart::NuclearSource);
			layout.SetSlot(4, ShipPart::Empty);
			layout.SetSlot(5, ShipPart::Hull);
			layout.SetSlot(6, ShipPart::Blocked);
			break;
		case ShipType::Starbase:
			layout.SetSlot(4, ShipPart::Blocked);
			fixedInitiative = 2;
			break;
		}
		break;
	case RaceType::Orion:
		fixedInitiative += 1;
		switch (type)
		{
		case ShipType::Interceptor:	
			layout.SetSlot(1, ShipPart::GaussShield);
			fixedPower = 1;
			break;
		case ShipType::Cruiser:
			layout.SetSlot(4, ShipPart::GaussShield);
			fixedPower = 2;
			break;
		case ShipType::Dreadnought:
			layout.SetSlot(5, ShipPart::GaussShield);
			fixedPower = 3;
			break;
		case ShipType::Starbase:
			layout.SetSlot(4, ShipPart::GaussShield);
			break;
		}
		break;
	}

	return BlueprintPtr(new Blueprint(layout, fixedInitiative, fixedPower, fixedComputer));
}

int Race::GetBuildCost(Buildable b) const
{
	int discount = m_type == RaceType::Mechanema ? 1 : 0;
	switch (b)
	{
	case Buildable::Starbase:
	case Buildable::Interceptor:	return 3 - discount;
	case Buildable::Cruiser:		return 5 - discount;
	case Buildable::Dreadnought:	return 7 - discount;
	case Buildable::Orbital:		return 5 - discount;
	case Buildable::Monolith:		return 10 - discount * 2;
	}
	return  0;
}

std::vector<TechType> Race::GetStartTechnologies() const
{
	std::vector<TechType> techs;

	switch (m_type)
	{
	case RaceType::Planta:
	case RaceType::Human:		techs.push_back(TechType::StarBase); break;
	case RaceType::Eridani:		techs.push_back(TechType::PlasmaCannon); techs.push_back(TechType::GaussShield); techs.push_back(TechType::FusionDrive); break;
	case RaceType::Hydran:		techs.push_back(TechType::AdvLabs); break;
	case RaceType::Mechanema:	techs.push_back(TechType::PositronComp); break;
	case RaceType::Orion:		techs.push_back(TechType::NeutronBomb); techs.push_back(TechType::GaussShield); break;
	}
	return techs;
}

int Race::GetTradeRate() const
{
	return m_type == RaceType::Human ? 2 : m_type == RaceType::Orion ? 4 : 3;
}

int Race::GetResearchRate() const
{
	return m_type == RaceType::Hydran ? 2 : 1;
}

int Race::GetBuildRate() const
{
	return m_type == RaceType::Mechanema ? 3 : 2;
}

int Race::GetUpgradeRate() const
{
	return m_type == RaceType::Mechanema ? 3 : 2;
}

int Race::GetExploreRate() const
{
	return m_type == RaceType::Planta ? 2 : 1;
}

int Race::GetExploreChoices() const
{
	return m_type == RaceType::Descendants ? 2 : 1;
}

bool Race::IsPopulationAutoDestroyed() const
{
	return m_type == RaceType::Planta;
}

int Race::GetExtraVictoryPointsPerHex() const
{
	return m_type == RaceType::Planta ? 1 : 0;
}

bool Race::IsAncientsAlly() const
{
	return m_type == RaceType::Descendants;
}

Colour Race::GetColour() const
{
	assert(m_type != RaceType::Human);

	switch (m_type)
	{
	case RaceType::Eridani:		return Colour::Red;
	case RaceType::Hydran:		return Colour::Blue;
	case RaceType::Planta:		return Colour::Green;
	case RaceType::Descendants:	return Colour::Yellow;
	case RaceType::Mechanema:	return Colour::White;
	case RaceType::Orion:		return Colour::Black;
	}
	return Colour::None;
}

DEFINE_ENUM_NAMES(RaceType) { "Human", "Eridani", "Hydran", "Planta", "Descendants", "Mechanema", "Orion", "" };
