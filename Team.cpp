#include "stdafx.h"
#include "Team.h"
#include "Race.h"
#include "Game.h"
#include "EnumRange.h"
#include "EnumTraits.h"
#include "Games.h"
#include "Players.h"
#include "Serial.h"

Team::Team() : m_idGame(0), m_idPlayer(0), m_race(RaceType::None), m_colour(Colour::None), m_nColonyShipsUsed(0), m_repTrack(*this)
{
	for (int i = 0; i < 4; ++i)
		m_nShips[i] = 0;
}

Team::Team(int idGame, int idPlayer) :
	m_idGame(idGame), m_idPlayer(idPlayer), m_race(RaceType::None), m_colour(Colour::None), m_nColonyShipsUsed(0), m_repTrack(*this)
{
	for (int i = 0; i < 4; ++i)
		m_nShips[i] = 0;
}

Team::Team(const Team& rhs, int idGame) : 
	m_idGame(idGame), m_idPlayer(rhs.m_idPlayer), m_race(rhs.m_race), m_colour(rhs.m_colour), m_allies(rhs.m_allies),
	m_popTrack(rhs.m_popTrack), m_infTrack(rhs.m_infTrack), m_repTrack(rhs.m_repTrack), m_techTrack(rhs.m_techTrack), 
	m_storage(rhs.m_storage), m_nColonyShips(rhs.m_nColonyShips), m_nColonyShipsUsed(rhs.m_nColonyShipsUsed)
{	
	for (int i = 0; i < 4; ++i)
		m_nShips[i] = rhs.m_nShips[i];
}

void Team::Assign(RaceType race, Colour colour)
{
	AssertThrowModel("Team::Assign", m_race == RaceType::None);
	m_race = race;
	m_colour = colour;

	Race r(race);
	m_storage = r.GetStartStorage();
	m_nColonyShips = r.GetStartColonyShips();
	
	m_infTrack.AddDiscs(r.GetStartInfluenceDiscs());

	std::vector<TechType> techs = r.GetStartTechnologies();
	for (TechType t : techs)
		m_techTrack.Add(t);

	for (int i = 0; i < r.GetStartReputationTiles(); ++i)
		m_repTrack.AddReputationTile(Games::Get(m_idGame).GetReputationBag().TakeTile());
	
	for (auto i : EnumRange<ShipType>())
		m_blueprints[(int)i].reset(new Blueprint(race, i));

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

const Player& Team::GetPlayer() const
{
	return Players::Get(m_idPlayer);
}

Player& Team::GetPlayer() 
{
	return Players::Get(m_idPlayer);
}

const Game& Team::GetGame() const
{
	return Games::Get(m_idGame);
}

void Team::AddShips(ShipType type, int nShips)
{
	m_nShips[(int)type] += nShips;
}

void Team::PopulateStartHex(Hex& hex)
{
	hex.SetColour(m_colour);
	for (Square* pSquare : hex.GetAvailableSquares(*this))
	{
		m_popTrack.Remove(SquareTypeToResource(pSquare->GetType()), 1);
		pSquare->SetOccupied(true);
	}
 
	ShipType ship = Race(m_race).GetStartShip();
	hex.AddShip(ship, m_colour);
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

const Blueprint& Team::GetBlueprint(ShipType s) const
{
	AssertThrowModel("Team::GetBlueprint", int(s) >= 0 && s!= ShipType::_Count);
	return *m_blueprints[int(s)];
}

bool Team::IsAncientAlliance(const Team* pTeam1, const Team* pTeam2)
{
	AssertThrowModel("Team::IsAncientAlliance", pTeam1 != pTeam2);
	return (!pTeam1 || !pTeam2) && Race((pTeam1 ? pTeam1 : pTeam2)->GetRace()).IsAncientsAlly();
}

void Team::Save(Serial::SaveNode& node) const
{
	node.SaveType("player", m_idPlayer);
	node.SaveEnum("race", m_race);
	node.SaveEnum("colour", m_colour);
	node.SaveCntr("allies", m_allies, Serial::EnumSaver());

	node.SaveClass("pop_track", m_popTrack);
	node.SaveClass("inf_track", m_infTrack);
	node.SaveClass("rep_track", m_repTrack);
	node.SaveClass("tech_track", m_techTrack);
	node.SaveClass("storage", m_storage);

	node.SaveArray("blueprints", m_blueprints, Serial::ClassPtrSaver());
	node.SaveArray("ships", m_nShips, Serial::TypeSaver());

	node.SaveType("colony_ships", m_nColonyShips);
	node.SaveType("colony_ships_used", m_nColonyShipsUsed);
}

void Team::Load(const Serial::LoadNode& node)
{
	node.LoadType("player", m_idPlayer);
	node.LoadEnum("race", m_race);
	node.LoadEnum("colour", m_colour);
	node.LoadCntr("allies", m_allies, Serial::EnumLoader());

	node.LoadClass("pop_track", m_popTrack);
	node.LoadClass("inf_track", m_infTrack);
	node.LoadClass("rep_track", m_repTrack);
	node.LoadClass("tech_track", m_techTrack);
	node.LoadClass("storage", m_storage);

	node.LoadArray("blueprints", m_blueprints, Serial::ClassPtrLoader());
	node.LoadArray("ships", m_nShips, Serial::TypeLoader());

	node.LoadType("colony_ships", m_nColonyShips);
	node.LoadType("colony_ships_used", m_nColonyShipsUsed);

	if (m_race != RaceType::None)
		for (auto s : EnumRange<ShipType>())
			m_blueprints[(int)s]->Init(m_race, s);
}

DEFINE_ENUM_NAMES2(Colour, -1) { "None", "Red", "Blue", "Green", "Yellow", "White", "Black", "" };
DEFINE_ENUM_NAMES(Buildable) { "Interceptor", "Cruiser", "Dreadnought", "Starbase", "Orbital", "Monolith", "" };
