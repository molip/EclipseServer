#pragma once

#include "ShipLayout.h"
#include "App.h"

#include <map>

enum class RaceType;
enum class ShipType;

typedef std::pair<RaceType, ShipType> BlueprintID;

class BlueprintDef
{
	friend class BlueprintDefs;
public:	
	BlueprintDef(const ShipLayout& layout, int fixedInitiative, int fixedPower, int fixedComputer);

	const ShipLayout& GetBaseLayout() const { return m_layout; }
	
	int GetFixedInitiative() const { return m_fixedInitiative; }
	int GetFixedPower() const { return m_fixedPower; }
	int GetFixedComputer() const { return m_fixedComputer; }

private:
	const ShipLayout m_layout;
	const int m_fixedInitiative, m_fixedPower, m_fixedComputer;
};

DEFINE_UNIQUE_PTR(BlueprintDef)

class BlueprintDefs
{
public:
	static const BlueprintDef& Get(RaceType r, ShipType s);

	static const BlueprintDef& GetAncientShip();
	static const BlueprintDef& GCDS();

private:
	BlueprintDefs();
	static const BlueprintDefs& Instance();
	BlueprintDef* AddBlueprintDef(BlueprintID id);
	
	std::map<BlueprintID, BlueprintDefPtr> m_map;
};
