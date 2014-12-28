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

void ActionRecord::Apply(bool bDo, Team& team, TeamState& teamState, const RecordContext& context)
{
	teamState.GetInfluenceTrack().RemoveDiscs(bDo ? 1 : -1);
	teamState.GetActionTrack().AddDiscs(bDo ? 1 : -1);
	context.SendMessage(Output::UpdateInfluenceTrack(team));
	context.SendMessage(Output::UpdateActionTrack(team));
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
