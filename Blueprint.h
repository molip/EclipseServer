#pragma once

#include "ShipLayout.h"

#include <memory>

class BlueprintDef;
class SlotRange;
class Dice;
enum class RaceType;

namespace Serial { class SaveNode; class LoadNode; }

class Blueprint : public ISlots
{
public:
	Blueprint();
	Blueprint(RaceType r, ShipType s);
	Blueprint(const BlueprintDef& def);
	Blueprint(const Blueprint& rhs);

	void Init(RaceType r, ShipType s);

	ShipType GetType() const;
	const ShipLayout& GetBaseLayout() const;
	const ShipLayout& GetOverlay() const { return m_overlay; }
	
	int GetFixedInitiative() const;
	int GetFixedPower() const;
	int GetFixedComputer() const;

	int GetInitiative() const;
	int GetPowerSource() const;
	int GetPowerDrain() const;
	int GetComputer() const;
	int GetShield() const;
	int GetMovement() const;
	int GetExtraHulls() const;
	int GetLives() const;

	bool HasCannon() const;
	bool HasMissiles() const;
	void AddDice(Dice& dice, bool missiles) const;

	virtual int GetSlotCount() const override { return m_overlay.GetSlotCount(); }
	void SetSlot(int i, ShipPart part) { m_overlay.SetSlot(i, part); }
	virtual ShipPart GetSlot(int i) const override;

	bool IsValid() const;

	static const Blueprint& GetAncientShip();
	static const Blueprint& GetGCDS();

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	const BlueprintDef* m_pDef;
	ShipLayout m_overlay;
};

typedef std::unique_ptr<Blueprint> BlueprintPtr;
