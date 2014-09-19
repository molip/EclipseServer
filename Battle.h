#pragma once

#include <vector>
#include <map>

enum class ShipType;
enum class Colour;
class LiveGame;
class Hex;
class Dice;
class Ship;
class Blueprint;
class Game;

namespace Serial { class SaveNode; class LoadNode; }

class Battle
{
public:

	struct Group
	{
		ShipType shipType;
		bool invader;
		bool hasMissiles; // false if they've been used.

		std::vector<int> ships; // Damage taken per ship.

		void Save(Serial::SaveNode& node) const;
		void Load(const Serial::LoadNode& node);
	};

//	typedef std::map<ShipType, int> Targets; // Dice roll required to hit ship type.

	Battle();
	Battle(const Hex& hex, Colour defender, Colour invader, const LiveGame& game);

	//const ShipVec& GetDefenderShips() const { return m_defenderShips; }
	//const ShipVec& GetInvaderShips() const { return m_invaderShips; }

	const std::vector<Group>& GetGroups() const { return m_groups; }

	const Group& GetCurrentGroup() const { return m_groups[m_groupIndex]; }

	const Blueprint& GetBlueprint(const Game& game, const Group& group) const;
	const Blueprint& GetBlueprint(const Game& game, ShipType shipType, bool invader) const;
	const Blueprint& GetCurrentBlueprint(const Game& game) const;

	//Colour GetDefenderColour() const { return m_defender; }
	//Colour GetInvaderColour() const { return m_invader; }
	Colour GetColour(bool invader) const { return invader ? m_invader : m_defender; }
	Colour GetCurrentTeamColour() const { return GetColour(GetCurrentGroup().invader); }

	
	bool IsMissilePhase() const { return m_missilePhase; }
	
	void RollDice(const LiveGame& game, Dice& dice) const;
	//Targets GetTargets() const;

	void AdvanceTurn();

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	int FindFirstMissileGroup() const;
	Group& GetCurrentGroup() { return m_groups[m_groupIndex]; }

	Colour m_defender, m_invader;
	std::vector<Group> m_groups;
	size_t m_groupIndex;
	bool m_missilePhase;
};
