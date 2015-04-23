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

ReputationTrack::ReputationTrack() : m_team(nullptr)
{
}

ReputationTrack::ReputationTrack(const ReputationTrack& rhs) : 
m_team(nullptr), m_repTiles(rhs.m_repTiles)
{
}

bool ReputationTrack::operator==(const ReputationTrack& rhs) const
{
	return m_repTiles == rhs.m_repTiles;
}

void ReputationTrack::SetTeam(const Team& team)
{
	VERIFY_MODEL(!m_team);
	m_team = &team;
}

ReputationSlots ReputationTrack::GetSlots() const
{
	return Race(m_team->GetRace()).GetReputationSlots();
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
	int nEitherSlotsUsed = std::max(0, (int)m_team->GetAllies().size() - slots.GetCount(ReputationType::Ambassador));
	return nAmbassadorSlots + nEitherSlotsUsed;
}

int ReputationTrack::GetEmptyReputationTileSlots() const
{
	return GetSlotCount() - GetFirstReputationTileSlot() - GetReputationTileCount();
}

bool ReputationTrack::CanAddReputationTile(int val) const
{
	VERIFY(val >= 1 && val <= 4);

	if (GetEmptyReputationTileSlots() > 0)
		return true;
	
	VERIFY(!m_repTiles.empty());
	return val > m_repTiles.back();
}

int ReputationTrack::AddReputationTile(int val)
{
	VERIFY(CanAddReputationTile(val));

	int old = 0;

	if (GetEmptyReputationTileSlots() == 0)
	{
		old = m_repTiles.back();
		m_repTiles.pop_back();
	}

	m_repTiles.push_back(val);
	std::sort(m_repTiles.begin(), m_repTiles.end(), std::greater<int>());

	return old;
}

void ReputationTrack::RemoveReputationTile(int val)
{
	auto it = std::find(m_repTiles.begin(), m_repTiles.end(), val);
	VERIFY_MODEL(it != m_repTiles.end());
	m_repTiles.erase(it);
}

bool ReputationTrack::CanAddAmbassador() const
{
	ReputationSlots slots = GetSlots();
	return (int)m_team->GetAllies().size() < slots.GetCount(ReputationType::Ambassador) + slots.GetCount(ReputationType::Either);
}

bool ReputationTrack::OnAmbassadorAdded()
{
	int nEmptyRepTileSlots = GetEmptyReputationTileSlots();
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
