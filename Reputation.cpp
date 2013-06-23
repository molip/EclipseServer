#include "stdafx.h"
#include "Reputation.h"
#include "Team.h"
#include "App.h"
#include "EnumTraits.h"
#include "Serial.h"
#include "Race.h"

#include <algorithm>

ReputationSlots::ReputationSlots(int a, int e, int r)
{
	m_types[(int)ReputationType::Ambassador] = a;
	m_types[(int)ReputationType::Either] = e;
	m_types[(int)ReputationType::Reputation] = r;
}

//-----------------------------------------------------------------------------

ReputationTrack::ReputationTrack(const Team& team) : m_team(team)
{
}

ReputationTrack::ReputationTrack(const ReputationTrack& rhs, const Team& team) : 
	m_team(team), m_repTiles(rhs.m_repTiles)

{
}

ReputationSlots ReputationTrack::GetSlots() const
{
	return Race(m_team.GetRace()).GetReputationSlots();
}

int ReputationTrack::GetSlotCount() const
{
	ReputationSlots slots = GetSlots();
	return slots.GetCount(ReputationType::Ambassador) + slots.GetCount(ReputationType::Either) + slots.GetCount(ReputationType::Reputation);
}

ReputationType ReputationTrack::GetSlotType(int iSlot) const
{
	ReputationSlots slots = GetSlots();
	for (int i = 0; i < 3; ++i)
	{
		iSlot -= slots.GetCount(ReputationType(i));
		if (iSlot < 0)
			return ReputationType(i);
	}
	ASSERT(false);
	return ReputationType::Ambassador;
}

int ReputationTrack::GetFirstReputationTileSlot() const
{
	ReputationSlots slots = GetSlots();

	int nAmbassadorSlots = slots.GetCount(ReputationType::Ambassador);
	int nEitherSlotsUsed = std::max(0, (int)m_team.GetAllies().size() - slots.GetCount(ReputationType::Ambassador));
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
	ReputationSlots slots = GetSlots();
	return (int)m_team.GetAllies().size() < slots.GetCount(ReputationType::Ambassador) + slots.GetCount(ReputationType::Either);
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

void ReputationTrack::Save(Serial::SaveNode& node) const
{
	node.SaveCntr("tiles", m_repTiles, Serial::TypeSaver());
}

void ReputationTrack::Load(const Serial::LoadNode& node)
{
	node.LoadCntr("tiles", m_repTiles, Serial::TypeLoader());
}

DEFINE_ENUM_NAMES(ReputationType) { "Ambassador", "Either", "Reputation", "" };
