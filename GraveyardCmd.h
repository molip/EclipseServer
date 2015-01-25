#pragma once

#include "MovePopulationCommand.h"
#include "Types.h"

#include <vector> 
#include <set> 

class GraveyardCmd : public MovePopulationCommand
{
public:
	GraveyardCmd() {}
	GraveyardCmd(Colour colour, const LiveGame& game);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
};
