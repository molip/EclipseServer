#pragma once

#include "Blueprint.h"
#include "ShipLayout.h"

enum class ShipType { Interceptor, Cruiser, Dreadnought, Starbase, _Count };

class Ship
{
public:
	Ship(const Blueprint& blueprint);

	ShipType GetType() const { return m_blueprint.GetType(); }
	const Blueprint& GetBlueprint() const { return m_blueprint; }
	int GetExtraInitiative() const { return m_extraInitiative; }
	int GetExtraPower() const { return m_extraPower; }
	int GetExtraComputer() const { return m_extraComputer; }

private:
	Blueprint m_blueprint;
	ShipLayout m_layout;
	int m_extraInitiative, m_extraPower, m_extraComputer;
};

