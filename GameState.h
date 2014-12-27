#pragma once

#include "Map.h"

#include <map>

class Game;
class Battle;
struct TeamState;

DEFINE_UNIQUE_PTR(Battle)
DEFINE_UNIQUE_PTR(TeamState)

struct GameState
{
	GameState(Game& game);
	~GameState();
	GameState(const GameState& rhs) = delete;
	GameState(const GameState& rhs, Game& game);
	bool operator==(const GameState& rhs) const;

	TeamState& GetTeamState(Colour c);
	const TeamState& GetTeamState(Colour c) const { return const_cast<GameState*>(this)->GetTeamState(c); }

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

	std::map<TechType, int> m_techs;
	Map	m_map; // After m_discBag
	int m_iRound;
	BattlePtr m_battle;

	std::map<Colour, TeamStatePtr> m_teamStates;
};
