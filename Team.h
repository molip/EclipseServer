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
enum class ScoreCategory { Reputation, Ambassador, Hex, Discovery, Monolith, Technology, Traitor, Race, _Count };

enum class RaceType;
enum class ShipPart;

class Score : public EnumIntArray<ScoreCategory>
{
public:
	Score() : m_total(-100), m_storage(0) {}
	bool operator<(const Score& rhs) 
	{
		if (GetTotal() < rhs.GetTotal())
			return true;
		return m_storage < rhs.m_storage;
	}

	int GetTotal() const
	{
		if (m_total == -100)
			m_total = __super::GetTotal();
		return m_total;
	}

	int GetStorage() const { return m_storage; }
	void SetStorage(int val) { m_storage = val; }

private:
	mutable int m_total;
	int m_storage;
};



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

	void Assign(RaceType race, Colour colour, LiveGame& game);
	bool IsAssigned() const;

	const int GetPlayerID() const { return m_idPlayer; }
	const Player& GetPlayer() const;
	Player& GetPlayer();

	const RaceType& GetRace() const { return m_race; }
	Colour GetColour() const { return m_colour; }

	const std::set<Colour> GetAllies() const { return m_state->m_allies; }

	bool HasTech(TechType tech) const { return m_state->m_techTrack.Has(tech); }
	bool CanAdd(TechType tech) const { return m_state->m_techTrack.CanAdd(tech); }

	const Storage& GetStorage() const { return m_state->m_storage; }
	const PopulationTrack& GetPopulationTrack() const { return m_state->m_popTrack; }
	const ReputationTrack& GetReputationTrack() const { return m_state->m_repTrack; }
	const InfluenceTrack& GetInfluenceTrack() const { return m_state->m_infTrack; }
	const DiscTrack& GetActionTrack() const { return m_state->m_actionTrack; }
	const TechTrack& GetTechTrack() const { return m_state->m_techTrack; }

	const SquareCounts& GetGraveyard() const { return m_state->m_graveyard; }

	int GetUnusedShips(ShipType type) const;

	Storage GetIncome() const;
	bool IsBankrupt() const;

	void PopulateStartHex(Hex& hex);

	int GetColonyShips() const;
	int GetUsedColonyShips() const { return m_state->m_nColonyShipsUsed; }
	int GetUnusedColonyShips() const { return GetColonyShips() - m_state->m_nColonyShipsUsed; }

	int GetVictoryPointTiles() const { return m_state->m_victoryPointTiles; }

	bool HasPassed() const { return m_state->m_bPassed; }

	const Blueprint& GetBlueprint(ShipType s) const { return m_state->GetBlueprint(s); }
	bool CanUseShipPart(ShipPart part) const;

	static bool IsAncientAlliance(const Team* pTeam1, const Team* pTeam2);

	Score GetScore(const Game& game) const;

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	int m_idPlayer;
	RaceType m_race;
	Colour m_colour;

	TeamState* m_state;
};

typedef std::unique_ptr<Team> TeamPtr;

