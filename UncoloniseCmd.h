#pragma once

#include "MovePopulationCommand.h"
#include "Types.h"
#include "MapPos.h"

#include <vector> 
#include <set> 

class UncoloniseCmd : public MovePopulationCommand
{
public:
	UncoloniseCmd() {}
	UncoloniseCmd(Colour colour, const MapPos& pos);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	SquareCounts GetSquareCounts(const LiveGame& game) const;

	MapPos m_pos;
};
