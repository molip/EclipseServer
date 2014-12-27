#include "stdafx.h"
#include "Blueprint.h"
#include "BlueprintDefs.h"
#include "ShipLayout.h"
#include "Ship.h"
#include "Serial.h"
#include "Dice.h"

Blueprint::Blueprint() : m_pDef(nullptr)
{
}

Blueprint::Blueprint(RaceType r, ShipType s) : m_pDef(nullptr)
{
	Init(r, s);
	m_overlay.SetType(s);

	for (int i = 0; i < GetSlotCount(); ++i)
		if (GetBaseLayout().GetSlot(i) == ShipPart::Blocked)
			m_overlay.SetSlot(i, ShipPart::Blocked);
}

Blueprint::Blueprint(const BlueprintDef& def) : m_pDef(&def), m_overlay(m_pDef->GetBaseLayout().GetType())
{
}

Blueprint::Blueprint(const Blueprint& rhs) : m_pDef(rhs.m_pDef), m_overlay(rhs.m_overlay)
{
}

bool Blueprint::operator==(const Blueprint& rhs) const
{
	return m_overlay == rhs.m_overlay;
}

void Blueprint::Init(RaceType r, ShipType s) 
{
	m_pDef = &BlueprintDefs::Get(r, s);
}

ShipType Blueprint::GetType() const { return m_pDef->GetBaseLayout().GetType(); }
const ShipLayout& Blueprint::GetBaseLayout() const { return m_pDef->GetBaseLayout(); }
	
int Blueprint::GetFixedInitiative() const { return m_pDef->GetFixedInitiative(); }
int Blueprint::GetFixedPower() const { return m_pDef->GetFixedPower(); }
int Blueprint::GetFixedComputer() const { return m_pDef->GetFixedComputer(); }

int Blueprint::GetInitiative() const
{
	int n = GetFixedInitiative();
	for (auto s : SlotRange(*this))
		n += ShipLayout::GetInitiative(s);
	return n;
}

int Blueprint::GetPowerSource() const
{
	int n = GetFixedPower();
	for (auto s : SlotRange(*this))
		n += ShipLayout::GetPowerSource(s);
	return n;
}

int Blueprint::GetPowerDrain() const
{
	int n = 0;
	for (auto s : SlotRange(*this))
		n += ShipLayout::GetPowerDrain(s);
	return n;
}

int Blueprint::GetComputer() const
{
	int n = GetFixedComputer();
	for (auto s : SlotRange(*this))
		n += ShipLayout::GetComputer(s);
	return n;
}

int Blueprint::GetShield() const
{
	int n = 0;
	for (auto s : SlotRange(*this))
		n += ShipLayout::GetShield(s);
	return n;
}

int Blueprint::GetMovement() const
{
	int n = 0;
	for (auto s : SlotRange(*this))
		n += ShipLayout::GetMovement(s);
	return n;
}

int Blueprint::GetExtraHulls() const
{
	int n = 0;
	for (auto s : SlotRange(*this))
		n += ShipLayout::GetHulls(s);
	return n;
}

int Blueprint::GetLives() const
{
	return GetExtraHulls() + 1;
}

bool Blueprint::HasCannon() const
{
	for (auto s : SlotRange(*this))
		if (s == ShipPart::IonCannon || s == ShipPart::PlasmaCannon || 
			s == ShipPart::AntimatterCannon || s == ShipPart::IonTurret)
			return true;
	return false;
}

bool Blueprint::HasMissiles() const
{
	for (auto s : SlotRange(*this))
		if (s == ShipPart::PlasmaMissile || s == ShipPart::IonMissile)
			return true;
	return false;
}

void Blueprint::AddDice(Dice& dice, bool missiles) const
{
	for (auto s : SlotRange(*this))
	{ 
		if (missiles)
		{
			switch (s)
			{
			case ShipPart::IonMissile:		dice.Add(DiceColour::Yellow, 3); break;
			case ShipPart::PlasmaMissile:	dice.Add(DiceColour::Orange, 2); break;
			}
		}
		else
		{
			switch (s)
			{
			case ShipPart::IonCannon:		dice.Add(DiceColour::Yellow, 1); break;
			case ShipPart::PlasmaCannon:	dice.Add(DiceColour::Orange, 1); break;
			case ShipPart::AntimatterCannon:dice.Add(DiceColour::Red, 1);	 break;
			case ShipPart::IonTurret:		dice.Add(DiceColour::Yellow, 2); break;
			}
		}
	}
}

ShipPart Blueprint::GetSlot(int i) const
{
	ShipPart overlay = m_overlay.GetSlot(i);
	return overlay == ShipPart::Empty ? m_pDef->GetBaseLayout().GetSlot(i) : overlay;
}

bool Blueprint::IsValid() const
{
	if (GetPowerDrain() > GetPowerSource())
		return false;

	int movement = GetMovement();
	return (GetType() == ShipType::Starbase) == (movement == 0);
}

const Blueprint& Blueprint::GetAncientShip()
{
	static Blueprint blueprint(BlueprintDefs::GetAncientShip());
	return blueprint;
}

const Blueprint& Blueprint::GetGCDS()
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
