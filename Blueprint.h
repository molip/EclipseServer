#pragma once

#include "ShipLayout.h"

#include <memory>

class BlueprintDef;

enum class RaceType;

namespace Serial { class SaveNode; class LoadNode; }

class Blueprint
{
public:
	Blueprint();
	Blueprint(RaceType r, ShipType s);
	Blueprint(const BlueprintDef& def);

	void Init(RaceType r, ShipType s);

	ShipType GetType() const;
	const ShipLayout& GetBaseLayout() const;
	
	int GetFixedInitiative() const;
	int GetFixedPower() const;
	int GetFixedComputer() const;

	int GetInitiative() const;
	int GetPowerSource() const;
	int GetPowerDrain() const;
	int GetComputer() const;
	int GetShield() const;
	int GetMovement() const;
	int GetHulls() const;

	int GetSlotCount() const { return m_overlay.GetSlotCount(); }
	void SetSlot(int i, ShipPart part) { m_overlay.SetSlot(i, part); }
	ShipPart GetSlot(int i) const;

	static const Blueprint& GetAncientShip();
	static const Blueprint& GetGCDS();

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	const BlueprintDef* m_pDef;
	ShipLayout m_overlay;
};

typedef std::unique_ptr<Blueprint> BlueprintPtr;
