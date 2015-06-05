#pragma once

#include "Battle.h"



class PopulationBattle : public Battle
{
public:
	struct Hits : public std::vector<int>
	{
		Hits(bool _autoHit = false) : autoHit(_autoHit) {}
		void Save(Serial::SaveNode& node) const;
		void Load(const Serial::LoadNode& node);

		bool autoHit;
	};

	PopulationBattle();
	PopulationBattle(const Hex& hex, const Game& game, const GroupVec& oldGroups);
	virtual bool operator==(const Battle& rhs) const override;
	virtual BattlePtr Clone() const override;

	virtual RecordPtr CreateAttackRecord(const Game& game, const Dice& dice) const override;
	virtual RecordPtr CreateAutoAttackRecord(const Game& game) const override;
	virtual bool IsPopulationBattle() const override { return true; }

private:
	virtual void DoAdvanceTurn(const Game& game) override;

	bool CanAutoDestroyPopulation(const Game& game) const;
	Hits AutoAssignPopulationHits(const Dice& dice, const Game& game) const;
};
