#pragma once

#include "Record.h"

class ActionRecord : public TeamRecord
{
public:
	ActionRecord();
	ActionRecord(const std::string& actionName, Colour colour);
	
private:
	virtual void Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState);
	virtual void Update(const Game& game, const Team& team, const RecordContext& context) const override;
	virtual std::string GetTeamMessage() const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	std::string m_actionName;
};
