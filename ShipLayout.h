#pragma once

#include <vector>

enum class ShipType;

enum class ShipPart {	Empty = 0, Blocked, 
						IonCannon, PlasmaCannon, AntimatterCannon, 
						PlasmaMissile, 
						ElectronComp, GluonComp, PositronComp, 
						NuclearDrive, TachyonDrive, FusionDrive,
						NuclearSource, TachyonSource, FusionSource, 
						PhaseShield, GaussShield, 
						Hull, ImprovedHull };

class ShipLayout
{
public:
	ShipLayout(ShipType type);

	ShipType GetType() const { return m_type; }
	int GetSlotCount() const { return m_slots.size(); }
	ShipPart GetSlot(int i) const { return m_slots[i]; }
	void SetSlot(int i, ShipPart part) { m_slots[i] = part; }

private:
	ShipType m_type; 
	std::vector<ShipPart> m_slots;
};

