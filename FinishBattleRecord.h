#pragma once

#include "Record.h"

class Battle;
DEFINE_UNIQUE_PTR(Battle)

class FinishBattleRecord : public Record
{
public:
	FinishBattleRecord();
	
private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller);
	virtual std::string GetMessage(const Game& game) const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	BattlePtr m_battle;
};
