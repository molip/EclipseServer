#include "stdafx.h"
#include "Team.h"
#include "Race.h"
#include "Game.h"
#include "EnumRange.h"

Team::Team(Game& game, const std::string player, RaceType race, Colour colour) : 
	m_game(game), m_player(player), m_race(race), m_colour(colour), m_nColonyShipsUsed(0), m_repTrack(*this)
{
	Race r(race);
	m_storage = r.GetStartStorage();
	m_nColonyShips = r.GetStartColonyShips();
	
	m_infTrack.AddDiscs(r.GetStartInfluenceDiscs());

	std::vector<TechType> techs = r.GetStartTechnologies();
	for (TechType t : techs)
		m_techTrack.Add(t);

	m_repTrack.SetSlots(r.GetReputationSlots());

	for (int i = 0; i < r.GetStartReputationTiles(); ++i)
		m_repTrack.AddReputationTile(m_game.GetReputationBag().TakeTile());
	
	for (auto i : EnumRange<ShipType>())
		m_blueprints[(int)i] = r.GetBlueprint(i);

	m_nShips[(int)ShipType::Interceptor] = 8;
	m_nShips[(int)ShipType::Cruiser] = 4;
	m_nShips[(int)ShipType::Dreadnought] = 2;
	m_nShips[(int)ShipType::Starbase] = 4;

	//ShipType GetStartShip() const;
	//int GetStartSector(Colour colour) const;
}

Team::~Team()
{
}
