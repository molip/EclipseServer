#pragma once

#include "MovePopulationCommand.h"
#include "Record.h"

class MovePopulationRecord : public TeamRecord
{
public:
	MovePopulationRecord();
	MovePopulationRecord(Colour colour, const MovePopulationCommand::Moves& moves);
	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

protected:
	void Update(const Game& game, const Team& team, const RecordContext& context) const;

	MovePopulationCommand::Moves m_moves;
};
