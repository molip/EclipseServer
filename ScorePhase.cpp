#include "stdafx.h"
#include "ScorePhase.h"
#include "LiveGame.h"
#include "CmdStack.h"
#include "Cmd.h"
#include "Controller.h"
#include "Output.h"
#include "Player.h"
#include "StartRoundRecord.h"
#include "CommitSession.h"
#include "GraveyardCmd.h"
#include "IncomeRecord.h"
#include "InfluenceRecord.h"
#include "AutoInfluenceCmd.h"

ScorePhase::ScorePhase(LiveGame* pGame) : Phase(pGame)
{
}

CmdStack& ScorePhase::GetCmdStack(Colour c)
{
	VERIFY(false);
	return *static_cast<CmdStack*>(nullptr);
}

REGISTER_DYNAMIC(ScorePhase)
