#pragma once

#include "Phase.h"

class ScorePhase : public Phase
{
public:
	ScorePhase(LiveGame* pGame = nullptr);

protected:
	virtual CmdStack& GetCmdStack(Colour c) override;

private:
};

