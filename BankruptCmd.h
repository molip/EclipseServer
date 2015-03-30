#pragma once

#include "Cmd.h"
#include "MapPos.h"

#include <vector> 

class BankruptCmd : public Cmd
{
public:
	BankruptCmd() {}
	BankruptCmd(Colour colour, const LiveGame& game);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;

private:
	std::vector<MapPos> GetSources(const LiveGame& game) const;
};
