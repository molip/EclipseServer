#pragma once

#include "Battle.h"



class ShipBattle : public Battle
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

	ShipBattle();
	ShipBattle(const Hex& hex, const Game& game, const GroupVec& oldGroups);
	virtual bool operator==(const Battle& rhs) const override;
	virtual BattlePtr Clone() const override;

	virtual RecordPtr CreateAttackRecord(const Game& game, const Dice& dice) const override;

	// Non-const.
	Group* FindTargetGroup(ShipType shipType);

private:
	std::vector<int> GetShipIndicesWeakestFirst(const Group& group) const; // Dead ships ignored. 
	std::vector<int> GetTargetGroupIndicesBiggestFirst() const; // Dead groups included.
	Hits GetHitsToDestroy(const Group& group, Dice& hitDice, int toHit) const;
	Hits AutoAssignHits(const Dice& dice, const Game& game) const;
	virtual void DoAdvanceTurn(const Game& game) override;
};
