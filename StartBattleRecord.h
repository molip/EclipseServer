#pragma once

#include "Record.h"
#include "Battle.h"

class StartBattleRecord : public Record
{
public:
	StartBattleRecord() : m_hexId(0) {}
	StartBattleRecord(const Battle* oldBattle, int hexId);
	
private:
	virtual void Apply(bool bDo, const Game& game, GameState& gameState);
	virtual void Update(const Game& game, const RecordContext& context) const override;
	virtual std::string GetMessage(const Game& game) const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	Battle::GroupVec m_oldGroups;
	int m_hexId;
};
