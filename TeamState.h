#pragma once

#include "TechTrack.h"
#include "InfluenceTrack.h"
#include "ActionTrack.h"
#include "PopulationTrack.h"
#include "Reputation.h"
#include "App.h"
#include "Blueprint.h"

#include <array>
#include <set>

class Team;
class Blueprint;
enum class Colour;
enum class ShipPart;

DEFINE_UNIQUE_PTR(Blueprint)

struct TeamState
{
	TeamState();
	TeamState(const TeamState& rhs);
	bool operator==(const TeamState& rhs) const;

	void SetTeam(const Team& team);

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

	std::set<Colour> m_allies;

	PopulationTrack m_popTrack;
	InfluenceTrack m_infTrack;
	ActionTrack m_actionTrack;
	ReputationTrack m_repTrack;
	TechTrack m_techTrack;
	Storage m_storage;

	std::array<BlueprintPtr, 4> m_blueprints;
	int m_nShips[4];

	std::set<ShipPart> m_discoveredShipParts;

	int m_nColonyShipsUsed;
	bool m_bPassed;
};
