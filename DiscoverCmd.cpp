#include "stdafx.h"
#include "DiscoverCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "Player.h"
#include "Race.h"
#include "Team.h"
#include "LiveGame.h"

DiscoverCmd::DiscoverCmd(Player& player, DiscoveryType discovery) : 
	Cmd(player), m_discovery(discovery)
{
}

void DiscoverCmd::UpdateClient(const Controller& controller) const
{
	Output::ChooseDiscovery msg(GetGame().CanRemoveCmd());
	controller.SendMessage(msg, m_player);
}

CmdPtr DiscoverCmd::Process(const Input::CmdMessage& msg, const Controller& controller)
{
	auto& m = CastThrow<const Input::CmdExploreDiscovery>(msg);

	return GetNextCmd();
}

void DiscoverCmd::Undo(const Controller& controller)
{
}
