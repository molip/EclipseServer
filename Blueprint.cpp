#include "stdafx.h"
#include "Blueprint.h"
#include "BlueprintDefs.h"
#include "ShipLayout.h"
#include "Ship.h"
#include "Serial.h"

Blueprint::Blueprint() : m_pDef(nullptr)
{
}

Blueprint::Blueprint(RaceType r, ShipType s) : m_pDef(nullptr)
{
	Init(r, s);
}

Blueprint::Blueprint(const BlueprintDef& def) : m_pDef(&def), m_overlay(m_pDef->GetBaseLayout().GetType())
{
}

void Blueprint::Init(RaceType r, ShipType s) 
{
	m_pDef = &BlueprintDefs::Get(r, s);
	m_overlay.SetType(s);
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

void Blueprint::Save(Serial::SaveNode& node) const
{
	node.SaveClass("overlay", m_overlay);
}

void Blueprint::Load(const Serial::LoadNode& node)
{
	node.LoadClass("overlay", m_overlay);
}
