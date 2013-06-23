#pragma once

#include <vector>

enum class ReputationType { Ambassador, Either, Reputation, _Count };

namespace Serial { class SaveNode; class LoadNode; }

class ReputationSlots
{
public:
	ReputationSlots(int a, int e, int r);

	int GetCount(ReputationType t) const { return m_types[(int)t]; }

private:
	int m_types[3];
};

class Team;

class ReputationTrack
{
public:
	ReputationTrack(const Team& team);
	ReputationTrack(const ReputationTrack& rhs, const Team& team);

	int GetSlotCount() const;
	ReputationType GetSlotType(int iSlot) const;

	int GetFirstReputationTileSlot() const;
	int GetReputationTileCount() const { return m_repTiles.size(); }
	int GetReputationTile(int i) const { return m_repTiles[i]; }

	bool AddReputationTile(int val);
	bool CanAddAmbassador() const;
	bool OnAmbassadorAdded();

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	ReputationSlots GetSlots() const;

	std::vector<int> m_repTiles;
	const Team& m_team;
};
