#pragma once

#include "Technology.h"
#include "TechTrack.h"
#include "InfluenceTrack.h"
#include "PopulationTrack.h"
#include "Race.h"
#include "Resources.h"
#include "Reputation.h"
#include "Blueprint.h"
#include "Ship.h"

#include <vector>
#include <set>
#include <memory>

enum class Colour { None = -1, Red, Blue, Green, Yellow, White, Black, _Count };
enum class Buildable { Interceptor, Cruiser, Dreadnought, Starbase, Orbital, Monolith, _Count };

enum class RaceType;

class Game;
class Hex;
class Player;

class Team
{
public:
	Team(int idGame, int idPlayer);
	~Team();

	void Team::Assign(RaceType race, Colour colour);

	const int GetPlayerID() const { return m_idPlayer; }
	const Player& GetPlayer() const;
	const RaceType& GetRace() const { return m_race; }
	Colour GetColour() const { return m_colour; }

	const std::set<Team*> GetAllies() const { return m_allies; }

	void AddTech(TechType tech) { m_techTrack.Add(tech); }
	bool HasTech(TechType tech) const { return m_techTrack.Has(tech); }
	bool CanAdd(TechType tech) const { return m_techTrack.CanAdd(tech); }

	Storage& GetStorage() { return m_storage; }
	const Storage& GetStorage() const { return m_storage; }
	
	PopulationTrack& GetPopulationTrack() { return m_popTrack; }
	const PopulationTrack& GetPopulationTrack() const { return m_popTrack; }

	ReputationTrack& GetReputationTrack() { return m_repTrack; }
	const ReputationTrack& GetReputationTrack() const { return m_repTrack; }

	InfluenceTrack& GetInfluenceTrack() { return m_infTrack; }
	const InfluenceTrack& GetInfluenceTrack() const { return m_infTrack; }

	const TechTrack& GetTechTrack() const { return m_techTrack; }

	void AddShips(ShipType type, int nShips);
	void RemoveShips(ShipType type, int nShips) { AddShips(type, -nShips); }

	void PopulateStartHex(Hex& hex);

	void UseColonyShips(int nShips);
	void ReturnColonyShips(int nShips);
	int GetColonyShips() const { return m_nColonyShips; }
	int GetUsedColonyShips() const { return m_nColonyShipsUsed; }
	int GetUnusedColonyShips() const { return m_nColonyShips - m_nColonyShipsUsed; }

	static bool IsAncientAlliance(const Team* pTeam1, const Team* pTeam2);

private:
	int m_idGame, m_idPlayer;
	RaceType m_race;
	Colour m_colour;

	std::set<Team*> m_allies;

	PopulationTrack m_popTrack;
	InfluenceTrack m_infTrack;
	ReputationTrack m_repTrack;
	TechTrack m_techTrack;
	Storage m_storage;
	
	BlueprintPtr m_blueprints[4];
	int m_nShips[4];

	int m_nColonyShips, m_nColonyShipsUsed;
};

typedef std::unique_ptr<Team> TeamPtr;

