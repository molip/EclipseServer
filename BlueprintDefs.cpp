#include "stdafx.h"
#include "BlueprintDefs.h"
#include "App.h"
#include "Race.h"
#include "Ship.h"

BlueprintDef::BlueprintDef(const ShipLayout& layout, int fixedInitiative, int fixedPower, int fixedComputer) : 
	m_layout(layout), m_fixedInitiative(fixedInitiative), m_fixedPower(fixedPower), m_fixedComputer(fixedComputer)
{
}

//-----------------------------------------------------------------------------

BlueprintDefs::BlueprintDefs()
{
	RaceType races[] = { RaceType::Human, RaceType::Eridani, RaceType::Planta, RaceType::Orion };

	for (auto r : races)
	{
		Race race(r);
		for (auto s : PlayerShipTypesRange())
			m_map.insert(std::make_pair(BlueprintID(r, s), race.GetBlueprintDef(s)));
	}
}

const BlueprintDefs& BlueprintDefs::Instance()
{
	static BlueprintDefs defs;
	return defs;
}

const BlueprintDef& BlueprintDefs::Get(RaceType r, ShipType s)
{
	if (r != RaceType::Eridani && r != RaceType::Planta && r != RaceType::Orion)
		r = RaceType::Human;

	auto it = Instance().m_map.find(BlueprintID(r,  s));
	VerifyModel("BlueprintDefs::Get", it != Instance().m_map.end());
	return *it->second;
}

const BlueprintDef& BlueprintDefs::GetAncientShip()
{
	static BlueprintDefPtr p;
	if (!p)
	{
		ShipLayout layout(ShipType::Ancient);
		layout.SetSlot(0, ShipPart::IonCannon);
		layout.SetSlot(1, ShipPart::IonCannon);
		layout.SetSlot(2, ShipPart::Hull);
		p.reset(new BlueprintDef(layout, 2, 2, 1));
	}
	return *p;
}

const BlueprintDef& BlueprintDefs::GCDS()
{
	static BlueprintDefPtr p;
	if (!p)
	{
		ShipLayout layout(ShipType::GCDS);
		for (int i = 0; i < 4; ++i)
			layout.SetSlot(i, ShipPart::IonCannon);
		for (int i = 4; i < 11; ++i)
			layout.SetSlot(i, ShipPart::Hull);
		p.reset(new BlueprintDef(layout, 0, 4, 1));
	}
	return *p;
}