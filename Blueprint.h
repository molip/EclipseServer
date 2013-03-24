#pragma once

#include "ShipLayout.h"

#include <memory>

class Blueprint
{
public:
	Blueprint(const ShipLayout& layout, int fixedInitiative, int fixedPower, int fixedComputer);

	ShipType GetType() const { return m_base.GetType(); }
	const ShipLayout& GetBaseLayout() const { return m_base; }
	
	int GetFixedInitiative() const { return m_fixedInitiative; }
	int GetFixedPower() const { return m_fixedPower; }
	int GetFixedComputer() const { return m_fixedComputer; }

	static const Blueprint& GetAncientShip();
	static const Blueprint& GCDS();

private:
	const ShipLayout m_base;
	ShipLayout m_overlay;
	const int m_fixedInitiative, m_fixedPower, m_fixedComputer;
};

typedef std::unique_ptr<Blueprint> BlueprintPtr;
