#pragma once

#include "Record.h"
#include "Battle.h"

class StartBattleRecord : public Record
{
public:
	StartBattleRecord(const Battle* oldBattle = nullptr);
	
private:
	virtual void Apply(bool bDo, const RecordContext& context);
	virtual std::string GetMessage(const Game& game) const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	Battle::GroupVec m_oldGroups;
};
