#include "stdafx.h"
#include "Blueprint.h"
#include "ShipLayout.h"

Blueprint::Blueprint(const ShipLayout& layout, int fixedInitiative, int fixedPower, int fixedComputer) : 
	m_layout(layout), m_fixedInitiative(fixedInitiative), m_fixedPower(fixedPower), m_fixedComputer(fixedComputer)
{
}
