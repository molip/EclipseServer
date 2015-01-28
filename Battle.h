#pragma once

#include "Dice.h"
#include "App.h"
#include "Dynamic.h"

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

class Battle;
class Record;
DEFINE_UNIQUE_PTR(Battle);
DEFINE_UNIQUE_PTR(Record);

enum class BattlePhase { Missile, Main, Population };

class Battle : public Dynamic
{
public:
	struct Group
	{
		Group();
		Group(ShipType _shipType, bool _invader);
		bool operator==(const Group& rhs) const;

		ShipType shipType;
		bool invader;
		bool hasMissiles; // false if they've been used.

		std::vector<int> lifeCounts; // Remaining lives per ship. Dead if <= 0.

		bool IsDead() const;

		void Save(Serial::SaveNode& node) const;
		void Load(const Serial::LoadNode& node);
	};

	typedef std::vector<Group> GroupVec;

	struct Turn
	{
		Turn(int _groupIndex = 0, BattlePhase _phase = BattlePhase::Missile) : groupIndex(_groupIndex), phase(_phase) {}
		bool operator==(const Turn& rhs) const { return groupIndex == rhs.groupIndex && phase == rhs.phase; }

		int groupIndex;
		BattlePhase phase;

		void Save(Serial::SaveNode& node) const;
		void Load(const Serial::LoadNode& node);
	};

	Battle();
	Battle(const Hex& hex, const Game& game, const GroupVec& oldGroups);
	virtual bool operator==(const Battle& rhs) const;
	virtual BattlePtr Clone() const = 0;

	int GetHexId() const { return m_hexId; }

	const GroupVec& GetGroups() const { return m_groups; }

	const Group& GetCurrentGroup() const { return m_groups[m_turn.groupIndex]; }
	Group& GetCurrentGroup() { return m_groups[m_turn.groupIndex]; }
	int GetCurrentGroupIndex() const { return m_turn.groupIndex; }

	const Blueprint& GetBlueprint(const Game& game, const Group& group) const;
	const Blueprint& GetBlueprint(const Game& game, ShipType shipType, bool invader) const;
	const Blueprint& GetCurrentBlueprint(const Game& game) const;

	Colour GetColour(bool invader) const { return invader ? m_invader : m_defender; }
	Colour GetFiringColour() const { return GetColour(GetCurrentGroup().invader); }
	Colour GetTargetColour() const { return GetColour(!GetCurrentGroup().invader); }

	bool IsMissilePhase() const { return m_turn.phase == BattlePhase::Missile; }
	bool IsFinished() const { return m_turn.groupIndex < 0; }

	void RollDice(const LiveGame& game, Dice& dice) const;

	virtual RecordPtr CreateAttackRecord(const Game& game, const Dice& dice) const = 0;
	virtual RecordPtr CreateAutoAttackRecord(const Game& game) const;
	virtual bool IsPopulationBattle() const { return false; }

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	// Non-const.
	Turn AdvanceTurn(const Game& game); // Returns old turn.
	void SetTurn(const Turn& turn);

protected:
	void AddGroups(bool invader, const Hex& hex, const Game& game);
	void AddOldGroups(const GroupVec& oldGroups, const Hex& hex, const Game& game);
	int FindFirstMissileGroup() const;
	int GetToHitRoll(ShipType shipType, const Game& game) const;
	bool IsMainPhaseFinished() const;

	virtual void DoAdvanceTurn(const Game& game) = 0;

	Colour m_defender, m_invader;
	int m_hexId;
	
	GroupVec m_groups;

	Turn m_turn;
};
