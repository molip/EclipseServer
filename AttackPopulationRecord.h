#pragma once

#include "AttackRecord.h"
#include "Battle.h"


class AttackPopulationRecord : public AttackRecord
{
public:
	AttackPopulationRecord();
	AttackPopulationRecord(const Battle::PopulationHits& hits);
	
private:
	virtual void Apply(bool bDo, Game& game, const RecordContext& context);
	virtual std::string GetMessage(const Game& game) const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	int m_cubeCount;
	Battle::PopulationHits m_hits;
};
