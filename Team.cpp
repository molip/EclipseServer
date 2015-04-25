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

void Team::Assign(RaceType race, Colour colour, LiveGame& game)
{
	VERIFY_MODEL(!IsAssigned());
	m_race = race;
	m_colour = colour;
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

bool Team::IsBankrupt() const
{
	return GetStorage()[Resource::Money] + GetIncome()[Resource::Money] < 0;
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

Score Team::GetScore(const Game& game) const
{
	Score score;

	score[ScoreCategory::Reputation] = GetReputationTrack().GetReputationVictoryPoints();
	score[ScoreCategory::Ambassador] = GetReputationTrack().GetAmbassadorVictoryPoints();
	score[ScoreCategory::Hex] = game.GetMap().GetHexVictoryPoints(*this);
	score[ScoreCategory::Discovery] = m_state->m_victoryPointTiles * 2;
	score[ScoreCategory::Monolith] = game.GetMap().GetMonolithVictoryPoints(*this);
	score[ScoreCategory::Technology] = GetTechTrack().GetVictoryPoints();
	score[ScoreCategory::Traitor] = 0;
	score[ScoreCategory::Race] = game.GetMap().GetRaceVictoryPoints(*this);

	score.SetStorage(GetStorage().GetTotal());

	return score;
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
