#pragma once

#include "AttackRecord.h"
#include "Battle.h"

class AttackShipsRecord : public AttackRecord
{
public:
	AttackShipsRecord();
	AttackShipsRecord(const Battle::Hits& hits);
	
private:
	virtual void Apply(bool bDo, const Game& game, GameState& gameState);
	virtual void Update(const Game& game, const RecordContext& context) const override;
	virtual std::string GetMessage(const Game& game) const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	Battle::Hits m_hits;
	bool m_missilePhase;
	std::set<int> m_killIndices;
};
