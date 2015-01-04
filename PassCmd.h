#pragma once

#include "Cmd.h"

class PassCmd : public Cmd
{
public:
	PassCmd() {}
	PassCmd(Colour colour, const LiveGame& game);

	virtual bool IsAction() const { return true; } 
	virtual bool CostsInfluence() const { return false; } 
	virtual bool IsAutoProcess() const { return true; } 
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;
};

