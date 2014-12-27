#include "TeamState.h"

#include "Team.h"
#include "Serial.h"

TeamState::TeamState() : m_nColonyShipsUsed(0), m_bPassed(false)
{
	for (int i = 0; i < 4; ++i)
		m_nShips[i] = 0;
}

TeamState::TeamState(const TeamState& rhs) :
m_allies(rhs.m_allies), m_popTrack(rhs.m_popTrack), m_infTrack(rhs.m_infTrack), m_actionTrack(rhs.m_actionTrack), m_repTrack(rhs.m_repTrack),
m_techTrack(rhs.m_techTrack), m_storage(rhs.m_storage), m_nColonyShipsUsed(rhs.m_nColonyShipsUsed), m_bPassed(rhs.m_bPassed)
{
	for (int i = 0; i < 4; ++i)
	{
		m_nShips[i] = rhs.m_nShips[i];
		m_blueprints[i].reset(new Blueprint(*rhs.m_blueprints[i]));
	}
}

bool TeamState::operator==(const TeamState& rhs) const
{
	for (int i = 0; i < 4; ++i)
	{
		if (m_nShips[i] != rhs.m_nShips[i])
			return false;
		if (*m_blueprints[i] != *rhs.m_blueprints[i])
			return false;
	}

	if (m_allies == rhs.m_allies &&
		m_popTrack == rhs.m_popTrack &&
		m_infTrack == rhs.m_infTrack &&
		m_actionTrack == rhs.m_actionTrack &&
		m_repTrack == rhs.m_repTrack &&
		m_techTrack == rhs.m_techTrack &&
		m_storage == rhs.m_storage &&
		m_discoveredShipParts == rhs.m_discoveredShipParts &&
		m_nColonyShipsUsed == rhs.m_nColonyShipsUsed &&
		m_bPassed == rhs.m_bPassed)
		return true;

	return false;
}

void TeamState::SetTeam(const Team& team)
{
	m_repTrack.SetTeam(team);
}

void TeamState::Save(Serial::SaveNode& node) const
{
	node.SaveCntr("allies", m_allies, Serial::EnumSaver());
	node.SaveClass("pop_track", m_popTrack);
	node.SaveClass("inf_track", m_infTrack);
	node.SaveClass("action_track", m_actionTrack);
	node.SaveClass("rep_track", m_repTrack);
	node.SaveClass("tech_track", m_techTrack);
	node.SaveClass("storage", m_storage);
	node.SaveCntr("discovered_ship_parts", m_discoveredShipParts, Serial::EnumSaver());
	node.SaveArray("blueprints", m_blueprints, Serial::ClassPtrSaver());
	node.SaveArray("ships", m_nShips, Serial::TypeSaver());
	node.SaveType("colony_ships_used", m_nColonyShipsUsed);
	node.SaveType("passed", m_bPassed);
}

void TeamState::Load(const Serial::LoadNode& node)
{
	node.LoadCntr("allies", m_allies, Serial::EnumLoader());
	node.LoadClass("pop_track", m_popTrack);
	node.LoadClass("inf_track", m_infTrack);
	node.LoadClass("action_track", m_actionTrack);
	node.LoadClass("rep_track", m_repTrack);
	node.LoadClass("tech_track", m_techTrack);
	node.LoadClass("storage", m_storage);
	node.LoadCntr("discovered_ship_parts", m_discoveredShipParts, Serial::EnumLoader());
	node.LoadArray("blueprints", m_blueprints, Serial::ClassPtrLoader());
	node.LoadArray("ships", m_nShips, Serial::TypeLoader());
	node.LoadType("colony_ships_used", m_nColonyShipsUsed);
	node.LoadType("passed", m_bPassed);
}
