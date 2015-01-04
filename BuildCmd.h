#pragma once

#include "PhaseCmd.h"

#include <set>

enum class Buildable;

class BuildCmd : public PhaseCmd
{
public:
	BuildCmd() {}
	BuildCmd(Colour colour, const LiveGame& game, int iPhase = 0);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;
	virtual bool IsAction() const { return m_iPhase == 0; }
	virtual std::string GetActionName() const override { return "Build"; }
};

