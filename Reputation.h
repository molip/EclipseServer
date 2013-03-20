#pragma once

#include <vector>

enum class ReputationType { Ambassador, Either, Reputation };

class ReputationSlots
{
public:
	ReputationSlots() {}
	ReputationSlots(int a, int e, int r)
	{
		m_slots[(int)ReputationType::Ambassador] = a;
		m_slots[(int)ReputationType::Either] = e;
		m_slots[(int)ReputationType::Reputation] = r;
	}
	int GetCount(ReputationType t) const { return m_slots[(int)t]; }

private:
	int m_slots[3];
};

class Team;

class ReputationTrack
{
public:
	ReputationTrack(const Team& team);

	void SetSlots(const ReputationSlots& slots) { m_slots = slots; }

	int GetSlotCount() const;
	ReputationType GetSlotType(int iSlot) const;

	int GetFirstReputationTileSlot() const;
	int GetReputationTileCount() const { return m_repTiles.size(); }
	int GetReputationTile(int i) const { return m_repTiles[i]; }

	bool AddReputationTile(int val);
	bool CanAddAmbassador() const;
	bool OnAmbassadorAdded();

private:
	ReputationSlots m_slots;
	std::vector<int> m_repTiles;
	const Team& m_team;
};
