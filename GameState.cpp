#include "GameState.h"

#include "Battle.h"
#include "TeamState.h"
#include "Serial.h"
#include "Game.h"

GameState::GameState(Game& game) : m_map(game), m_iRound(-1)
{
	InitBags(game);
}

GameState::GameState(const GameState& rhs, Game& game) :
m_map(rhs.m_map, game), m_techs(rhs.m_techs), m_iRound(rhs.m_iRound),
m_battle(rhs.m_battle ? new Battle(*rhs.m_battle) : nullptr),
m_repBagState(rhs.m_repBagState), m_techBagState(rhs.m_techBagState), m_discBagState(rhs.m_discBagState), m_hexBagStates(rhs.m_hexBagStates)
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
	m_repBagState.SetBag(game.GetReputationBag());
	m_techBagState.SetBag(game.GetTechnologyBag());
	m_discBagState.SetBag(game.GetDiscoveryBag());
	for (auto&& ring : EnumRange<HexRing>())
		m_hexBagStates[ring].SetBag(game.GetHexBag(ring));
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

	VERIFY_MODEL(m_iRound <= 9);

	return m_iRound == 9;
}

Battle& GameState::GetBattle()
{
	VERIFY_MODEL(!!m_battle);
	return *m_battle;
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

void GameState::Save(Serial::SaveNode& node) const
{
	node.SaveClass("map", m_map);
	node.SaveMap("techs", m_techs, Serial::EnumSaver(), Serial::TypeSaver());
	node.SaveType("round", m_iRound);
	node.SaveClassPtr("battle", m_battle);
	node.SaveMap("team_states", m_teamStates, Serial::EnumSaver(), Serial::ClassPtrSaver());

	node.SaveClass("rep_bag", m_repBagState);
	node.SaveClass("tech_bag", m_techBagState);
	node.SaveClass("disc_bag", m_discBagState);
	node.SaveArray("hex_bag", m_hexBagStates, Serial::ClassSaver());
}

void GameState::Load(const Serial::LoadNode& node)
{
	node.LoadClass("map", m_map);
	node.LoadMap("techs", m_techs, Serial::EnumLoader(), Serial::TypeLoader());
	node.LoadType("round", m_iRound);
	node.LoadClassPtr("battle", m_battle);
	node.LoadMap("team_states", m_teamStates, Serial::EnumLoader(), Serial::ClassPtrLoader());

	node.LoadClass("rep_bag", m_repBagState);
	node.LoadClass("tech_bag", m_techBagState);
	node.LoadClass("disc_bag", m_discBagState);
	node.LoadArray("hex_bag", m_hexBagStates, Serial::ClassLoader());
}
