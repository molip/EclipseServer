#pragma once

#include "Record.h"
#include "Battle.h"

class AdvanceCombatTurnRecord : public Record
{
public:
	AdvanceCombatTurnRecord();
	
private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller);
	virtual std::string GetMessage(const Game& game) const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	Battle::Turn m_oldTurn;
};
