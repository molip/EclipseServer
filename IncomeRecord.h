#pragma once

#include "Record.h"
#include "Resources.h"

class IncomeRecord : public TeamRecord
{
public:
	IncomeRecord() {}
	IncomeRecord(Colour colour);

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	virtual void Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState) override;
	virtual void Update(const Game& game, const Team& team, const RecordContext& context) const override;
	virtual std::string GetTeamMessage() const override;

	Storage m_income;
};
