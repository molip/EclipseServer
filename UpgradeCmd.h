#pragma once

#include "Cmd.h"

class UpgradeCmd : public Cmd
{
public:
	UpgradeCmd() {}
	UpgradeCmd(Colour colour, const LiveGame& game);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;
	virtual bool IsAction() const { return true; }
	virtual std::string GetActionName() const override { return "Upgrade"; }

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
};

