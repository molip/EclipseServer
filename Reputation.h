#pragma once

#include <vector>

enum class ReputationType { Ambassador, Either, Reputation, _Count };

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
	ReputationTrack();
	ReputationTrack(const ReputationTrack& rhs);
	bool operator==(const ReputationTrack& rhs) const;

	void SetTeam(const Team& team);

	int GetSlotCount() const;
	ReputationType GetSlotType(int iSlot) const;

	int GetFirstReputationTileSlot() const;
	int GetReputationTileCount() const { return (int)m_repTiles.size(); }
	int GetReputationTile(int i) const { return m_repTiles[i]; }
	int GetReputationVictoryPoints() const;
	int GetAmbassadorVictoryPoints() const;

	bool CanAddReputationTile(int val) const;
	int AddReputationTile(int val);
	void RemoveReputationTile(int val);
	bool CanAddAmbassador() const;
	bool OnAmbassadorAdded();

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	ReputationSlots GetSlots() const;
	int GetEmptyReputationTileSlots() const;

	std::vector<int> m_repTiles;
	const Team* m_team;
};
