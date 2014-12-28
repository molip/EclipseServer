#include "GameState.h"

#include "Battle.h"
#include "TeamState.h"
#include "Serial.h"

GameState::GameState(Game& game) : m_map(game), m_iRound(-1)
{
}

GameState::GameState(const GameState& rhs, Game& game) :
m_map(rhs.m_map, game), m_techs(rhs.m_techs), m_iRound(rhs.m_iRound),
m_battle(rhs.m_battle ? new Battle(*rhs.m_battle) : nullptr)
{
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
void GameState::Save(Serial::SaveNode& node) const
{
	node.SaveClass("map", m_map);
	node.SaveMap("techs", m_techs, Serial::EnumSaver(), Serial::TypeSaver());
	node.SaveType("round", m_iRound);
	node.SaveClassPtr("battle", m_battle);
	node.SaveMap("team_states", m_teamStates, Serial::EnumSaver(), Serial::ClassPtrSaver());
}

void GameState::Load(const Serial::LoadNode& node)
{
	node.LoadClass("map", m_map);
	node.LoadMap("techs", m_techs, Serial::EnumLoader(), Serial::TypeLoader());
	node.LoadType("round", m_iRound);
	node.LoadClassPtr("battle", m_battle);
	node.LoadMap("team_states", m_teamStates, Serial::EnumLoader(), Serial::ClassPtrLoader());
}
