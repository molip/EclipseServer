#pragma once

#include "PhaseCmd.h"
#include "MapPos.h"

#include <vector> 

class Hex;

class AutoInfluenceCmd : public Cmd
{
public:
	AutoInfluenceCmd() {}
	AutoInfluenceCmd(Colour colour, const LiveGame& game);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;
	virtual std::string GetActionName() const override { return "Auto Influence"; }
	virtual bool CanUnstart() const { return false; }

	static Colour GetAutoInfluenceColour(const Hex& hex);

private:
	std::vector<MapPos> GetHexes(const LiveGame& game) const;
};
