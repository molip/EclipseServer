#include "stdafx.h"
#include "ActionRecord.h"
#include "Team.h"
#include "Game.h"
#include "Controller.h"
#include "Output.h"

ActionRecord::ActionRecord() {}

ActionRecord::ActionRecord(const std::string& actionName, Colour colour) : TeamRecord(colour), m_actionName(actionName)
{
}

void ActionRecord::Apply(bool bDo, Game& game, const Controller& controller)
{
	Team& team = game.GetTeam(m_colour);
	team.GetInfluenceTrack().RemoveDiscs(bDo ? 1 : -1);
	team.GetActionTrack().AddDiscs(bDo ? 1 : -1);
	controller.SendMessage(Output::UpdateInfluenceTrack(team), game);
	controller.SendMessage(Output::UpdateActionTrack(team), game);
}

std::string ActionRecord::GetTeamMessage() const
{
	return FormatString("started action: %0",  m_actionName);
}

void ActionRecord::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveType("action_name", m_actionName);
}

void ActionRecord::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadType("action_name", m_actionName);
}

REGISTER_DYNAMIC(ActionRecord)
