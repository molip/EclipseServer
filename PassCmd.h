#pragma once

#include "Cmd.h"

class PassCmd : public Cmd
{
public:
	PassCmd() {}
	PassCmd(Colour colour, LiveGame& game);

	virtual bool IsAction() const { return true; } 
	virtual bool CostsInfluence() const { return false; } 
	virtual bool IsAutoProcess() const { return true; } 
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game) override;
};

