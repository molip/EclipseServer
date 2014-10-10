#pragma once

#include "Dice.h"

#include <vector>
#include <map>

enum class ShipType;
enum class Colour;
class LiveGame;
class Hex;
class Ship;
class Blueprint;
class Game;

namespace Serial { class SaveNode; class LoadNode; }

class Battle
{
public:

	struct Hit
	{
	public:
		Hit();
		Hit(ShipType _shipType, int _shipIndex, const Dice& _dice);

		void Save(Serial::SaveNode& node) const;
		void Load(const Serial::LoadNode& node);

		ShipType shipType;
		int shipIndex;
		Dice dice;
	};

	struct Hits : public std::vector<Hit> 
	{
		void Save(Serial::SaveNode& node) const;
		void Load(const Serial::LoadNode& node);
	};

	struct Group
	{
		ShipType shipType;
		bool invader;
		bool hasMissiles; // false if they've been used.

		std::vector<int> lifeCounts; // Remaining lives per ship. Dead if <= 0.

		bool IsDead() const;

		void Save(Serial::SaveNode& node) const;
		void Load(const Serial::LoadNode& node);
	};

	struct Turn
	{
		Turn(int _groupIndex = 0, bool _missilePhase = false) : groupIndex(_groupIndex), missilePhase(_missilePhase) {}

		int groupIndex;
		bool missilePhase;

		void Save(Serial::SaveNode& node) const;
		void Load(const Serial::LoadNode& node);
	};

//	typedef std::map<ShipType, int> Targets; // Dice roll required to hit ship type.

	Battle();
	Battle(const Hex& hex, const Game& game);

	//const ShipVec& GetDefenderShips() const { return m_defenderShips; }
	//const ShipVec& GetInvaderShips() const { return m_invaderShips; }

	int GetHexId() const { return m_hexId; }

	const std::vector<Group>& GetGroups() const { return m_groups; }

	const Group& GetCurrentGroup() const { return m_groups[m_turn.groupIndex]; }
	Group& GetCurrentGroup() { return m_groups[m_turn.groupIndex]; }
	int GetCurrentGroupIndex() const { return m_turn.groupIndex; }

	const Blueprint& GetBlueprint(const Game& game, const Group& group) const;
	const Blueprint& GetBlueprint(const Game& game, ShipType shipType, bool invader) const;
	const Blueprint& GetCurrentBlueprint(const Game& game) const;

	Colour GetColour(bool invader) const { return invader ? m_invader : m_defender; }
	Colour GetFiringColour() const { return GetColour(GetCurrentGroup().invader); }
	Colour GetTargetColour() const { return GetColour(!GetCurrentGroup().invader); }

	bool IsMissilePhase() const { return m_turn.missilePhase; }
	bool IsFinished() const;

	void RollDice(const LiveGame& game, Dice& dice) const;
	Hits AutoAssignHits(const Dice& dice, const Game& game) const;

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

	// Non-const.
	Group* FindTargetGroup(ShipType shipType);
	void SetCurrentGroupIndex(int group);
	Turn AdvanceTurn(); // Returns old turn.
	void SetTurn(const Turn& turn);

private:
	int FindFirstMissileGroup() const;
	int GetToHitRoll(ShipType shipType, const Game& game) const;
	std::vector<int> GetShipIndicesWeakestFirst(const Group& group) const; // Dead ships ignored. 
	std::vector<int> GetTargetGroupIndicesBiggestFirst() const; // Dead groups included.
	Hits GetHitsToDestroy(const Group& group, Dice& hitDice, int toHit) const;

	Colour m_defender, m_invader;
	int m_hexId;
	
	std::vector<Group> m_groups;

	Turn m_turn;
};
