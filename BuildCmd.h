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
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller, const LiveGame& game) override;
	virtual bool IsAction() const { return m_iPhase == 0; } 

private:
	bool CanAfford(const LiveGame& game, Buildable b) const;
};

