#pragma once

#include "ShipLayout.h"

#include <memory>

class BlueprintDef;

enum class RaceType;

class Blueprint
{
public:
	Blueprint(RaceType r, ShipType s);
	Blueprint(const BlueprintDef& def);

	ShipType GetType() const;
	const ShipLayout& GetBaseLayout() const;
	
	int GetFixedInitiative() const;
	int GetFixedPower() const;
	int GetFixedComputer() const;

	static const Blueprint& GetAncientShip();
	static const Blueprint& GCDS();

private:
	const BlueprintDef* m_pDef;
	ShipLayout m_overlay;
};

typedef std::unique_ptr<Blueprint> BlueprintPtr;
