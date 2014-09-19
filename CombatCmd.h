#pragma once

#include "Cmd.h"

class CombatCmd : public Cmd
{
public:
	CombatCmd() {}
	CombatCmd(Colour colour, const LiveGame& game);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, CommitSession& session) override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
};

