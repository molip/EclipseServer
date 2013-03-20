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

enum class Colour { Black, Blue, Green, Red, White, Yellow, _Count };
enum class Buildable { Interceptor, Cruiser, Dreadnought, Starbase, Orbital, Monolith, _Count };

enum class RaceType;

class Game;
class Team
{
public:

	Team(Game& game, const std::string player, RaceType race, Colour colour);
	~Team();

	const std::string& GetPlayer() const { return m_player; }
	const RaceType& GetRace() const { return m_race; }
	Colour GetColour() const { return m_colour; }

	const std::set<Team*> GetAllies() const { return m_allies; }

private:
	Game& m_game;
	std::string m_player;
	RaceType m_race;
	Colour m_colour;

	std::set<Team*> m_allies;

	PopulationTrack m_popTrack;
	InfluenceTrack m_infTrack;
	ReputationTrack m_repTrack;
	TechTrack m_techTrack;
	Storage m_storage;
	
	BlueprintPtr m_blueprints[ShipType::_Count];
	int m_nShips[ShipType::_Count];

	int m_nColonyShips, m_nColonyShipsUsed;
};

typedef std::unique_ptr<Team> TeamPtr;

