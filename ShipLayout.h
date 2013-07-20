#pragma once

#include <vector>

namespace Serial { class SaveNode; class LoadNode; }

enum class ShipType;

enum class ShipPart {	Empty = 0, Blocked, 
						IonCannon, PlasmaCannon, AntimatterCannon, 
						PlasmaMissile, IonMissile,
						IonTurret, 
						ElectronComp, GluonComp, PositronComp, AxionComp,
						NuclearDrive, TachyonDrive, FusionDrive, ConformalDrive,
						NuclearSource, TachyonSource, FusionSource, HypergridSource,
						PhaseShield, GaussShield, FluxShield,
						Hull, ImprovedHull };

class ShipLayout
{
public:
	ShipLayout();
	ShipLayout(ShipType type);

	ShipType GetType() const { return m_type; }
	void SetType(ShipType t);
	int GetSlotCount() const { return m_slots.size(); }
	ShipPart GetSlot(int i) const { return m_slots[i]; }
	void SetSlot(int i, ShipPart part) { m_slots[i] = part; }

	static int GetInitiative(ShipPart p);
	static int GetPowerSource(ShipPart p);
	static int GetPowerDrain(ShipPart p);
	static int GetComputer(ShipPart p);
	static int GetShield(ShipPart p);
	static int GetMovement(ShipPart p);
	static int GetHulls(ShipPart p);

	static int GetSlotCount(ShipType t);

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	ShipType m_type; 
	std::vector<ShipPart> m_slots;
};

