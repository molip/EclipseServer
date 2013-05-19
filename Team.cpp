#include "stdafx.h"
#include "Team.h"
#include "Race.h"
#include "Game.h"
#include "EnumRange.h"

Team::Team(Game& game, Player& player, RaceType race, Colour colour) : 
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

	AddShips(ShipType::Interceptor, 8);
	AddShips(ShipType::Cruiser, 4);
	AddShips(ShipType::Dreadnought, 2);
	AddShips(ShipType::Starbase, 4);

	//ShipType GetStartShip() const;
	//int GetStartSector(Colour colour) const;
}

Team::~Team()
{
}

void Team::AddShips(ShipType type, int nShips)
{
	m_nShips[(int)type] += nShips;
}

void Team::PopulateStartHex(Hex& hex)
{
	hex.SetOwner(this);
	for (Square* pSquare : hex.GetAvailableSquares())
	{
		m_popTrack.Remove(SquareTypeToResource(pSquare->GetType()), 1);
		pSquare->SetOwner(this);
	}
 
	ShipType ship = Race(m_race).GetStartShip();
	hex.AddShip(ship, this);
	RemoveShips(ship, 1);

	m_infTrack.RemoveDiscs(1);
}

void Team::UseColonyShips(int nShips)
{
	AssertThrowModel("Team::UseColonyShips", m_nColonyShipsUsed + nShips <= m_nColonyShips);
	m_nColonyShipsUsed += nShips;
}

void Team::ReturnColonyShips(int nShips)
{
	m_nColonyShipsUsed = std::max(0, m_nColonyShipsUsed - nShips);
}

bool Team::IsAncientAlliance(const Team* pTeam1, const Team* pTeam2)
{
	AssertThrowModel("Team::IsAncientAlliance", pTeam1 != pTeam2);
	return (!pTeam1 || !pTeam2) && Race((pTeam1 ? pTeam1 : pTeam2)->GetRace()).IsAncientsAlly();
}
