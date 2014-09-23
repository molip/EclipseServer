#pragma once

#include "Record.h"

class StartBattleRecord : public Record
{
public:
	StartBattleRecord();
	
private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller);
	virtual std::string GetMessage(const Game& game) const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;
};
