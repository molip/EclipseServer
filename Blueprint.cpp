#include "stdafx.h"
#include "Blueprint.h"
#include "ShipLayout.h"
#include "Ship.h"

Blueprint::Blueprint(const ShipLayout& base, int fixedInitiative, int fixedPower, int fixedComputer) : 
	m_base(base), m_fixedInitiative(fixedInitiative), m_fixedPower(fixedPower), m_fixedComputer(fixedComputer), m_overlay(base.GetType())
{
}

const Blueprint& Blueprint::GetAncientShip()
{
	static BlueprintPtr p;
	if (!p)
	{
		ShipLayout layout(ShipType::Ancient);
		layout.SetSlot(0, ShipPart::IonCannon);
		layout.SetSlot(1, ShipPart::IonCannon);
		layout.SetSlot(2, ShipPart::Hull);
		p = BlueprintPtr(new Blueprint(layout, 2, 2, 1));
	}
	return *p.get();
}

const Blueprint& Blueprint::GCDS()
{
	static BlueprintPtr p;
	if (!p)
	{
		ShipLayout layout(ShipType::GCDS);
		for (int i = 0; i < 4; ++i)
			layout.SetSlot(1, ShipPart::IonCannon);
		for (int i = 4; i < 11; ++i)
			layout.SetSlot(1, ShipPart::Hull);
		p = BlueprintPtr(new Blueprint(layout, 0, 4, 1));
	}
	return *p.get();
}