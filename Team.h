#pragma once

#include "Technology.h"
#include "Race.h"
#include "Resources.h"
#include "Ship.h"
#include "TeamState.h"

#include <vector>
#include <set>
#include <memory>

enum class Colour { None = -1, Red, Blue, Green, Yellow, White, Black, _Count };
enum class Buildable { None = -1, Interceptor, Cruiser, Dreadnought, Starbase, Orbital, Monolith, _Count };

enum class RaceType;
enum class ShipPart;

namespace Serial { class SaveNode; class LoadNode; }

class Game;
class Hex;
class Player;
class LiveGame;

class Team
{
	friend class ReviewGame;
public:
	Team();
	Team(int idPlayer);
	Team(const Team& rhs);
	~Team();

	void SetState(TeamState& state);
	void InitState();

	void Assign(RaceType race, Colour colour, TeamState& state, LiveGame& game);
	bool IsAssigned() const;

	const int GetPlayerID() const { return m_idPlayer; }
	const Player& GetPlayer() const;
	Player& GetPlayer();
	//const Game& GetGame() const;

	const RaceType& GetRace() const { return m_race; }
	Colour GetColour() const { return m_colour; }

	const std::set<Colour> GetAllies() const { return m_state->m_allies; }

	void AddTech(TechType tech) { m_state->m_techTrack.Add(tech); }
	bool HasTech(TechType tech) const { return m_state->m_techTrack.Has(tech); }
	bool CanAdd(TechType tech) const { return m_state->m_techTrack.CanAdd(tech); }

	Storage& GetStorage() { return m_state->m_storage; }
	const Storage& GetStorage() const { return m_state->m_storage; }
	
	PopulationTrack& GetPopulationTrack() { return m_state->m_popTrack; }
	const PopulationTrack& GetPopulationTrack() const { return m_state->m_popTrack; }

	ReputationTrack& GetReputationTrack() { return m_state->m_repTrack; }
	const ReputationTrack& GetReputationTrack() const { return m_state->m_repTrack; }

	InfluenceTrack& GetInfluenceTrack() { return m_state->m_infTrack; }
	const InfluenceTrack& GetInfluenceTrack() const { return m_state->m_infTrack; }

	DiscTrack& GetActionTrack() { return m_state->m_actionTrack; }
	const DiscTrack& GetActionTrack() const { return m_state->m_actionTrack; }

	const TechTrack& GetTechTrack() const { return m_state->m_techTrack; }
	TechTrack& GetTechTrack() { return m_state->m_techTrack; }

	void AddShips(ShipType type, int nShips);
	void RemoveShips(ShipType type, int nShips) { AddShips(type, -nShips); }
	int GetUnusedShips(ShipType type) const;

	Storage GetIncome() const;

	void PopulateStartHex(Hex& hex);

	void UseColonyShips(int nShips);
	void ReturnColonyShips(int nShips);
	int GetColonyShips() const;
	int GetUsedColonyShips() const { return m_state->m_nColonyShipsUsed; }
	int GetUnusedColonyShips() const { return GetColonyShips() - m_state->m_nColonyShipsUsed; }

	void SetPassed(bool b) { m_state->m_bPassed = b; }
	bool HasPassed() const { return m_state->m_bPassed; }

	Blueprint& GetBlueprint(ShipType s);
	const Blueprint& GetBlueprint(ShipType s) const { return const_cast<Team*>(this)->GetBlueprint(s); }
	bool CanUseShipPart(ShipPart part) const;

	static bool IsAncientAlliance(const Team* pTeam1, const Team* pTeam2);

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	int m_idPlayer;
	RaceType m_race;
	Colour m_colour;

	TeamState* m_state;
};

typedef std::unique_ptr<Team> TeamPtr;

