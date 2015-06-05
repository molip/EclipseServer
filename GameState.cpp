#include "stdafx.h"
#include "GameState.h"

#include "ShipBattle.h"
#include "PopulationBattle.h"
#include "Team.h"
#include "TeamState.h"
#include "Game.h"

GameState::GameState(Game& game) : m_map(game), m_iRound(-1)
{
	InitBags(game);
}

GameState::GameState(const GameState& rhs, Game& game) :
m_map(rhs.m_map, game), m_techs(rhs.m_techs), m_iRound(rhs.m_iRound),
m_battle(rhs.m_battle ? rhs.m_battle->Clone() : nullptr),
m_repBag(rhs.m_repBag), m_techBagState(rhs.m_techBagState), m_discBagState(rhs.m_discBagState), m_hexPiles(rhs.m_hexPiles), m_hexDiscardPiles(rhs.m_hexDiscardPiles)
{
	InitBags(game);

	for (auto& pair : rhs.m_teamStates)
		m_teamStates.insert(std::make_pair(pair.first, TeamStatePtr(new TeamState(*pair.second))));
}

GameState::~GameState() {}

bool GameState::operator==(const GameState& rhs) const
{
	if (!ArePtrMapsEqual(m_teamStates, rhs.m_teamStates))
		return false;

	if (m_map == rhs.m_map &&
		m_techs == rhs.m_techs &&
		m_iRound == rhs.m_iRound &&
		(!m_battle && !rhs.m_battle || m_battle && rhs.m_battle && *m_battle == *rhs.m_battle))
		return true;

	return false;
}

void GameState::InitBags(const Game& game)
{
	m_techBagState.SetBag(game.GetTechnologyBag());
	m_discBagState.SetBag(game.GetDiscoveryBag());
}

void GameState::Init(const Game& game)
{
	Hex& centre = AddHex(MapPos(0, 0), 001, 0);
	centre.AddShip(ShipType::GCDS, Colour::None);

	// Initialise starting hexes.
	auto startPositions = m_map.GetTeamStartPositions();
	VERIFY(startPositions.size() == game.GetTeams().size());
	int i = 0;
	for (auto& team : game.GetTeams())
	{
		std::vector<int> repTiles;
		for (int j = 0; j < Race(team->GetRace()).GetStartReputationTiles(); ++j)
			repTiles.push_back(m_repBag.ChooseAndTakeTile());

		auto pair = m_teamStates.insert(std::make_pair(team->GetColour(), TeamStatePtr(new TeamState)));
		VERIFY(pair.second);
		team->SetState(*pair.first->second); // Set state first so Init can use team.
		auto& posRotPair = startPositions[i++];
		pair.first->second->Init(*team, posRotPair.first, posRotPair.second, m_map, repTiles);
	}
}

TeamState& GameState::GetTeamState(Colour c)
{
	auto i = m_teamStates.find(c);
	VERIFY_MODEL(i != m_teamStates.end());
	return *i->second;
}

bool GameState::IncrementRound(bool bDo)
{
	m_iRound += bDo ? 1 : -1;

	VERIFY_MODEL(m_iRound <= Game::RoundCount);

	return m_iRound == Game::RoundCount - 1 + bDo;
}

Battle& GameState::GetBattle()
{
	VERIFY_MODEL(!!m_battle);
	return *m_battle;
}

ShipBattle& GameState::GetShipBattle()
{
	ShipBattle* shipBattle = dynamic_cast<ShipBattle*>(m_battle.get());
	VERIFY_MODEL(shipBattle != nullptr);
	return *shipBattle;
}

PopulationBattle& GameState::GetPopulationBattle()
{
	PopulationBattle* populationBattle = dynamic_cast<PopulationBattle*>(m_battle.get());
	VERIFY_MODEL(populationBattle != nullptr);
	return *populationBattle;
}

void GameState::AttachBattle(BattlePtr battle)
{
	m_battle = std::move(battle);
}

BattlePtr GameState::DetachBattle()
{
	return std::move(m_battle);
}

Hex& GameState::AddHex(const MapPos& pos, int id, int rotation)
{
	VERIFY_MODEL_MSG("invalid rotation", rotation >= 0 && rotation < 6);
	HexPtr hex(new Hex(id, pos, rotation));

	if (hex->HasDiscovery())
		hex->SetDiscoveryTile(GetDiscoveryBag().TakeTile());

	return m_map.AddHex(std::move(hex));
}

void GameState::DeleteHex(const MapPos& pos)
{
	Hex* hex = m_map.FindHex(pos);
	VERIFY_MODEL_MSG("hex not found", !!hex);

	if (hex->HasDiscovery())
	{
		DiscoveryType d = hex->GetDiscoveryTile();
		VERIFY_MODEL_MSG("discovery tile has gone", d != DiscoveryType::None);
		DiscoveryType d2 = m_discBagState.ReturnTile();
		VERIFY_MODEL(d == d2);
	}
	m_map.DeleteHex(pos);
}

void GameState::ProduceTechnologies(int count, bool bDo)
{
	if (bDo)
	{
		VERIFY(count <= m_techBagState.GetRemaining());
		for (int i = 0; i < count; ++i)
			++m_techs[m_techBagState.TakeTile()];
	}
	else
	{
		VERIFY_MODEL(count <= m_techBagState.GetTaken());
		for (int i = 0; i < count; ++i)
			--m_techs[m_techBagState.ReturnTile()];
	}
}

void GameState::Save(Serial::SaveNode& node) const
{
	node.SaveClass("map", m_map);
	node.SaveMap("techs", m_techs, Serial::EnumSaver(), Serial::TypeSaver());
	node.SaveType("round", m_iRound);
	node.SaveObject("battle", m_battle);
	node.SaveMap("team_states", m_teamStates, Serial::EnumSaver(), Serial::ClassPtrSaver());

	node.SaveClass("rep_bag", m_repBag);
	node.SaveClass("tech_bag", m_techBagState);
	node.SaveClass("disc_bag", m_discBagState);
	node.SaveArray("hex_piles", m_hexPiles, Serial::ClassSaver());
	node.SaveArray("hex_discard_piles", m_hexDiscardPiles, Serial::ClassSaver());
}

void GameState::Load(const Serial::LoadNode& node)
{
	node.LoadClass("map", m_map);
	node.LoadMap("techs", m_techs, Serial::EnumLoader(), Serial::TypeLoader());
	node.LoadType("round", m_iRound);
	node.LoadObject("battle", m_battle);
	node.LoadMap("team_states", m_teamStates, Serial::EnumLoader(), Serial::ClassPtrLoader());

	node.LoadClass("rep_bag", m_repBag);
	node.LoadClass("tech_bag", m_techBagState);
	node.LoadClass("disc_bag", m_discBagState);
	node.LoadArray("hex_piles", m_hexPiles, Serial::ClassLoader());
	node.LoadArray("hex_discard_piles", m_hexDiscardPiles, Serial::ClassLoader());
}
