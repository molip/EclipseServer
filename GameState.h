#pragma once

#include "Map.h"

#include <map>

class Game;
class Battle;
class TeamState;

DEFINE_UNIQUE_PTR(Battle)
DEFINE_UNIQUE_PTR(TeamState)

class GameState
{
public:
	GameState(Game& game);
	~GameState();
	GameState(const GameState& rhs) = delete;
	GameState(const GameState& rhs, Game& game);
	bool operator==(const GameState& rhs) const;

	TeamState& GetTeamState(Colour c);
	const TeamState& GetTeamState(Colour c) const { return const_cast<GameState*>(this)->GetTeamState(c); }

	std::map<TechType, int>& GetTechnologies() { return m_techs; }
	Map& GetMap() { return m_map; }

	bool IncrementRound(bool bDo); // Returns true if game finished.
	Battle& GetBattle();
	void AttachBattle(BattlePtr battle);
	BattlePtr DetachBattle();

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

	std::map<TechType, int> m_techs;
	Map	m_map; // After m_discBag
	int m_iRound;
	BattlePtr m_battle;

	std::map<Colour, TeamStatePtr> m_teamStates;
};
