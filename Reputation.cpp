#include "stdafx.h"
#include "Reputation.h"
#include "Team.h"
#include "App.h"
#include "EnumTraits.h"

#include <algorithm>

ReputationTrack::ReputationTrack(const Team& team) : m_team(team)
{
}

int ReputationTrack::GetSlotCount() const
{
	return m_slots.GetCount(ReputationType::Ambassador) + m_slots.GetCount(ReputationType::Either) + m_slots.GetCount(ReputationType::Reputation);
}

ReputationType ReputationTrack::GetSlotType(int iSlot) const
{
	for (int i = 0; i < 3; ++i)
	{
		iSlot -= m_slots.GetCount(ReputationType(i));
		if (iSlot < 0)
			return ReputationType(i);
	}
	ASSERT(false);
	return ReputationType::Ambassador;
}

int ReputationTrack::GetFirstReputationTileSlot() const
{
	int nAmbassadorSlots = m_slots.GetCount(ReputationType::Ambassador);
	int nEitherSlotsUsed = std::max(0, (int)m_team.GetAllies().size() - m_slots.GetCount(ReputationType::Ambassador));
	return nAmbassadorSlots + nEitherSlotsUsed;
}

bool ReputationTrack::AddReputationTile(int val)
{
	int nEmptyRepTileSlots = GetSlotCount() - GetFirstReputationTileSlot() - GetReputationTileCount();

	if (nEmptyRepTileSlots > 0)
	{
		m_repTiles.push_back(val);
		std::sort(m_repTiles.begin(), m_repTiles.end(), std::greater<int>());
		return true;
	}

	if (!m_repTiles.empty() && val > m_repTiles.back())
	{
		m_repTiles.back() = val;
		return true;
	}
	return false;
}

bool ReputationTrack::CanAddAmbassador() const
{
	return (int)m_team.GetAllies().size() < m_slots.GetCount(ReputationType::Ambassador) + m_slots.GetCount(ReputationType::Either);
}

bool ReputationTrack::OnAmbassadorAdded()
{
	int nEmptyRepTileSlots = GetSlotCount() - GetFirstReputationTileSlot() - GetReputationTileCount();
	if (nEmptyRepTileSlots == -1)
	{
		m_repTiles.pop_back();
		return true;
	}
	ASSERT(nEmptyRepTileSlots >= 0);
	return false;

}

DEFINE_ENUM_NAMES(ReputationType) { "Ambassador", "Either", "Reputation" };
