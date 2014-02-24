#include "stdafx.h"
#include "ActionRecord.h"
#include "Team.h"
#include "Game.h"
#include "Controller.h"
#include "Output.h"

ActionRecord::ActionRecord() {}

ActionRecord::ActionRecord(Colour colour) : TeamRecord(colour) {}

void ActionRecord::Apply(bool bDo, Game& game, const Controller& controller)
{
	Team& team = game.GetTeam(m_colour);
	team.GetInfluenceTrack().RemoveDiscs(bDo ? 1 : -1);
	team.GetActionTrack().AddDiscs(bDo ? 1 : -1);
	controller.SendMessage(Output::UpdateInfluenceTrack(team), game);
	controller.SendMessage(Output::UpdateActionTrack(team), game);
}

REGISTER_DYNAMIC(ActionRecord)
