#include "stdafx.h"
#include "DiscoverCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "Player.h"
#include "Race.h"
#include "Team.h"
#include "LiveGame.h"
#include "ActionPhase.h"
#include "CommitSession.h"

DiscoverCmd::DiscoverCmd(Colour colour, const LiveGame& game, DiscoveryType discovery) : 
	Cmd(colour), m_discovery(discovery)
{
}

void DiscoverCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	Output::ChooseDiscovery msg(game.GetActionPhase().CanRemoveCmd());
	controller.SendMessage(msg, GetPlayer(game));
}

Cmd::ProcessResult DiscoverCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdExploreDiscovery>(msg);

	return ProcessResult(GetNextCmd(session.GetGame()));
}

void DiscoverCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveEnum("discovery", m_discovery);
}

void DiscoverCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadEnum("discovery", m_discovery);
}

REGISTER_DYNAMIC(DiscoverCmd)
