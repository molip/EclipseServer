#pragma once

#include "ShipLayout.h"

#include <memory>

class Blueprint
{
public:
	Blueprint(const ShipLayout& layout, int fixedInitiative, int fixedPower, int fixedComputer);

	ShipType GetType() const { return m_layout.GetType(); }
	const ShipLayout& GetLayout() const { return m_layout; }
	int GetExtraInitiative() const { return m_fixedInitiative; }
	int GetExtraPower() const { return m_fixedPower; }
	int GetExtraComputer() const { return m_fixedComputer; }

private:
	const ShipLayout m_layout;
	const int m_fixedInitiative, m_fixedPower, m_fixedComputer;
};

typedef std::unique_ptr<Blueprint> BlueprintPtr;
