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
class Map;
class MapPos;
enum class Colour;
enum class ShipPart;

DEFINE_UNIQUE_PTR(Blueprint)

class TeamState
{
public:
	TeamState();
	TeamState(const TeamState& rhs);
	bool operator==(const TeamState& rhs) const;

	void Init(const Team& team, const MapPos& pos, int rotation, Map& map, const std::vector<int>& repTiles);
	void SetTeam(const Team& team);

	Blueprint& GetBlueprint(ShipType s);
	Storage& GetStorage() { return m_storage; }
	PopulationTrack& GetPopulationTrack() { return m_popTrack; }
	ReputationTrack& GetReputationTrack() { return m_repTrack; }
	InfluenceTrack& GetInfluenceTrack() { return m_infTrack; }
	DiscTrack& GetActionTrack() { return m_actionTrack; }
	TechTrack& GetTechTrack() { return m_techTrack; }
	
	void SetPassed(bool b) { m_bPassed = b; }
	
	void AddShips(ShipType type, int nShips);
	void RemoveShips(ShipType type, int nShips) { AddShips(type, -nShips); }
	void UseColonyShips(int nShips);
	void ReturnColonyShips(int nShips);

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

private:
	void InitHex(const Team& team, const MapPos& pos, int rotation, Map& map);
};
