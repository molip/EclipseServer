#include "stdafx.h"
#include "Team.h"
#include "Race.h"
#include "LiveGame.h"
#include "EnumRange.h"
#include "EnumTraits.h"
#include "Games.h"
#include "Players.h"
#include "Serial.h"
#include "Blueprint.h"

Team::Team() : Team(0)
{
}

Team::Team(int idPlayer) :
m_idPlayer(idPlayer), m_race(RaceType::None), m_colour(Colour::None), m_state(nullptr)
{
}

Team::Team(const Team& rhs) :
m_idPlayer(rhs.m_idPlayer), m_race(rhs.m_race), m_colour(rhs.m_colour), m_state(nullptr)
{	
}

void Team::SetState(TeamState& state)
{
	VERIFY_MODEL(!m_state);
	m_state = &state;
	state.SetTeam(*this);
}

bool Team::IsAssigned() const
{
	return m_colour != Colour::None;
}

void Team::Assign(RaceType race, Colour colour, TeamState& state, LiveGame& game)
{
	VERIFY_MODEL(!IsAssigned());
	m_race = race;
	m_colour = colour;

	SetState(state);

	Race r(race);
	m_state->m_storage = r.GetStartStorage();
	
	m_state->m_infTrack.AddDiscs(r.GetStartInfluenceDiscs());

	std::vector<TechType> techs = r.GetStartTechnologies();
	for (TechType t : techs)
		m_state->m_techTrack.Add(t);

	for (int i = 0; i < r.GetStartReputationTiles(); ++i)
		m_state->m_repTrack.AddReputationTile(game.GetReputationBag().TakeTile());
	
	for (auto i : PlayerShipTypesRange())
		m_state->m_blueprints[(int)i].reset(new Blueprint(race, i));

	m_state->AddShips(ShipType::Interceptor, 8);
	m_state->AddShips(ShipType::Cruiser, 4);
	m_state->AddShips(ShipType::Dreadnought, 2);
	m_state->AddShips(ShipType::Starbase, 4);

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

int Team::GetUnusedShips(ShipType type) const 
{
	VERIFY_MODEL(int(type) >= 0);
	return m_state->m_nShips[(int)type];
}

Storage Team::GetIncome() const
{
	Storage income = m_state->m_popTrack.GetIncome();
	income[Resource::Money] -= m_state->m_infTrack.GetUpkeep();
	return income;
}

void Team::PopulateStartHex(Hex& hex)
{
	hex.SetColour(m_colour);
	for (Square* pSquare : hex.GetAvailableSquares(*this))
	{
		m_state->m_popTrack.Remove(SquareTypeToResource(pSquare->GetType()), 1);
		pSquare->SetOccupied(true);
	}
 
	ShipType ship = Race(m_race).GetStartShip();
	hex.AddShip(ship, m_colour);
	m_state->RemoveShips(ship, 1);

	m_state->m_infTrack.RemoveDiscs(1);
}

int Team::GetColonyShips() const
{
	return Race(m_race).GetStartColonyShips();
}

bool Team::CanUseShipPart(ShipPart part) const
{
	VERIFY_MODEL(int(part) >= 0);

	if (m_state->m_discoveredShipParts.find(part) != m_state->m_discoveredShipParts.end())
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

void Team::InitState()
{
	if (m_race != RaceType::None)
		for (auto s : PlayerShipTypesRange())
			m_state->m_blueprints[(int)s]->Init(m_race, s);
}

void Team::Save(Serial::SaveNode& node) const
{
	node.SaveType("player", m_idPlayer);
	node.SaveEnum("race", m_race);
	node.SaveEnum("colour", m_colour);
}

void Team::Load(const Serial::LoadNode& node)
{
	node.LoadType("player", m_idPlayer);
	node.LoadEnum("race", m_race);
	node.LoadEnum("colour", m_colour);
}

DEFINE_ENUM_NAMES2(Colour, -1) { "None", "Red", "Blue", "Green", "Yellow", "White", "Black", "" };
DEFINE_ENUM_NAMES2(Buildable, -1) { "None", "Interceptor", "Cruiser", "Dreadnought", "Starbase", "Orbital", "Monolith", "" };
