#include "stdafx.h"
#include "Blueprint.h"
#include "BlueprintDefs.h"
#include "ShipLayout.h"
#include "Ship.h"

Blueprint::Blueprint(RaceType r, ShipType s) : m_pDef(&BlueprintDefs::Get(r, s)), m_overlay(m_pDef->GetBaseLayout().GetType())
{
}

Blueprint::Blueprint(const BlueprintDef& def) : m_pDef(&def), m_overlay(m_pDef->GetBaseLayout().GetType())
{
}

ShipType Blueprint::GetType() const { return m_pDef->GetBaseLayout().GetType(); }
const ShipLayout& Blueprint::GetBaseLayout() const { return m_pDef->GetBaseLayout(); }
	
int Blueprint::GetFixedInitiative() const { return m_pDef->GetFixedInitiative(); }
int Blueprint::GetFixedPower() const { return m_pDef->GetFixedPower(); }
int Blueprint::GetFixedComputer() const { return m_pDef->GetFixedComputer(); }

const Blueprint& Blueprint::GetAncientShip()
{
	static Blueprint blueprint(BlueprintDefs::GetAncientShip());
	return blueprint;
}

const Blueprint& Blueprint::GCDS()
{
	static Blueprint blueprint(BlueprintDefs::GCDS());
	return blueprint;
}
