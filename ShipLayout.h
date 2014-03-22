#pragma once

#include "IndexRange.h"

#include <vector>

namespace Serial { class SaveNode; class LoadNode; }

enum class ShipType;

enum class ShipPart {	Empty = -2, Blocked, 
						IonCannon, PlasmaCannon, AntimatterCannon, 
						PlasmaMissile, IonMissile,
						IonTurret, 
						ElectronComp, GluonComp, PositronComp, AxionComp,
						NuclearDrive, TachyonDrive, FusionDrive, ConformalDrive,
						NuclearSource, TachyonSource, FusionSource, HypergridSource,
						PhaseShield, GaussShield, FluxShield,
						Hull, ImprovedHull, 
						_Count };

class SlotRange;

class ISlots
{
public:
	virtual int GetSlotCount() const = 0;
	virtual	ShipPart GetSlot(int i) const = 0;

	SlotRange GetSlotRange() const;
};

DEFINE_INDEXRANGE(SlotRange, ISlots, ShipPart, GetSlot, GetSlotCount)

class ShipLayout : public ISlots
{
public:
	ShipLayout();
	ShipLayout(ShipType type);

	ShipType GetType() const { return m_type; }
	void SetType(ShipType t);
	virtual int GetSlotCount() const override { return m_slots.size(); }
	virtual ShipPart GetSlot(int i) const override { return m_slots[i]; }
	void SetSlot(int i, ShipPart part);

	static int GetInitiative(ShipPart p);
	static int GetPowerSource(ShipPart p);
	static int GetPowerDrain(ShipPart p);
	static int GetComputer(ShipPart p);
	static int GetShield(ShipPart p);
	static int GetMovement(ShipPart p);
	static int GetHulls(ShipPart p);
	
	static bool IsDrive(ShipPart p) { return GetMovement(p) > 0; }

	static int GetSlotCount(ShipType t);

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	ShipType m_type; 
	std::vector<ShipPart> m_slots;
};
