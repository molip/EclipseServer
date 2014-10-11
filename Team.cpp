#include "stdafx.h"
#include "Team.h"
#include "Race.h"
#include "LiveGame.h"
#include "EnumRange.h"
#include "EnumTraits.h"
#include "Games.h"
#include "Players.h"
#include "Serial.h"

Team::Team() : Team(0) 
{
}

Team::Team(int idPlayer) :
	m_idPlayer(idPlayer), m_race(RaceType::None), m_colour(Colour::None), m_nColonyShipsUsed(0), 
	m_bPassed(false), m_repTrack(*this)
{
	for (int i = 0; i < 4; ++i)
		m_nShips[i] = 0;
}

Team::Team(const Team& rhs) : 
	m_idPlayer(rhs.m_idPlayer), m_race(rhs.m_race), m_colour(rhs.m_colour), m_allies(rhs.m_allies),
	m_popTrack(rhs.m_popTrack), m_infTrack(rhs.m_infTrack), m_actionTrack(rhs.m_actionTrack), m_repTrack(rhs.m_repTrack), 
	m_techTrack(rhs.m_techTrack), m_storage(rhs.m_storage), m_nColonyShipsUsed(rhs.m_nColonyShipsUsed), m_bPassed(rhs.m_bPassed)
{	
	for (int i = 0; i < 4; ++i)
	{
		m_nShips[i] = rhs.m_nShips[i];
		m_blueprints[i].reset(new Blueprint(*rhs.m_blueprints[i]));
	}
}

void Team::Assign(RaceType race, Colour colour, LiveGame& game)
{
	VERIFY_MODEL(m_race == RaceType::None);
	m_race = race;
	m_colour = colour;

	Race r(race);
	m_storage = r.GetStartStorage();
	
	m_infTrack.AddDiscs(r.GetStartInfluenceDiscs());

	std::vector<TechType> techs = r.GetStartTechnologies();
	for (TechType t : techs)
		m_techTrack.Add(t);

	for (int i = 0; i < r.GetStartReputationTiles(); ++i)
		m_repTrack.AddReputationTile(game.GetReputationBag().TakeTile());
	
	for (auto i : PlayerShipTypesRange())
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

void Team::AddShips(ShipType type, int nShips)
{
	// TODO: Check max count not exceeded.
	VERIFY_MODEL_MSG("invalid ship type", int(type) >= 0);
	m_nShips[(int)type] += nShips;
}

int Team::GetUnusedShips(ShipType type) const 
{
	VERIFY_MODEL(int(type) >= 0);
	return m_nShips[(int)type]; 
}

Storage Team::GetIncome() const
{
	Storage income = m_popTrack.GetIncome();
	income[Resource::Money] -= m_infTrack.GetUpkeep();
	return income;
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
	VERIFY_MODEL(m_nColonyShipsUsed + nShips <= GetColonyShips());
	m_nColonyShipsUsed += nShips;
}

void Team::ReturnColonyShips(int nShips)
{
	m_nColonyShipsUsed = std::max(0, m_nColonyShipsUsed - nShips);
}

int Team::GetColonyShips() const
{
	return Race(m_race).GetStartColonyShips();
}

Blueprint& Team::GetBlueprint(ShipType s) 
{
	VERIFY_MODEL(int(s) >= 0 && s != ShipType::_Count);
	return *m_blueprints[int(s)];
}

bool Team::CanUseShipPart(ShipPart part) const
{
	VERIFY_MODEL(int(part) >= 0);

	if (m_discoveredShipParts.find(part) != m_discoveredShipParts.end())
		return true;

	static ShipPart free[] = { ShipPart::ElectronComp, ShipPart::Hull, ShipPart::IonCannon, ShipPart::NuclearDrive, ShipPart::NuclearSource };
	if (std::find(std::begin(free), std::end(free), part) != std::end(free))
		return true;

	static std::pair<ShipPart, TechType> partTechs[] = 
	{
		{ ShipPart::PlasmaCannon,		TechType::PlasmaCannon },
		{ ShipPart::AntimatterCannon,	TechType::AntimatterCannon },
		{ ShipPart::PlasmaMissile,		TechType::PlasmaMissile },
		{ ShipPart::GluonComp,			TechType::GluonComp },
		{ ShipPart::PositronComp,		TechType::PositronComp },
		{ ShipPart::TachyonDrive,		TechType::TachyonDrive },
		{ ShipPart::FusionDrive,		TechType::FusionDrive },
		{ ShipPart::TachyonSource,		TechType::TachyonSource },
		{ ShipPart::FusionSource,		TechType::FusionSource },
		{ ShipPart::PhaseShield,		TechType::PhaseShield },
		{ ShipPart::GaussShield,		TechType::GaussShield },
		{ ShipPart::ImprovedHull,		TechType::ImprovedHull }
	};

	for (auto pt : partTechs)
		if (part == pt.first && HasTech(pt.second))
			return true;

	return false;
}

bool Team::IsAncientAlliance(const Team* pTeam1, const Team* pTeam2)
{
	VERIFY_MODEL(pTeam1 != pTeam2);
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

void Team::Load(const Serial::LoadNode& node)
{
	node.LoadType("player", m_idPlayer);
	node.LoadEnum("race", m_race);
	node.LoadEnum("colour", m_colour);
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

	if (m_race != RaceType::None)
		for (auto s : PlayerShipTypesRange())
			m_blueprints[(int)s]->Init(m_race, s);
}

DEFINE_ENUM_NAMES2(Colour, -1) { "None", "Red", "Blue", "Green", "Yellow", "White", "Black", "" };
DEFINE_ENUM_NAMES2(Buildable, -1) { "None", "Interceptor", "Cruiser", "Dreadnought", "Starbase", "Orbital", "Monolith", "" };
