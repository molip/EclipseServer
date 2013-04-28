#include "stdafx.h"
#include "DiscoverCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "Player.h"
#include "Race.h"
#include "Team.h"

DiscoverCmd::DiscoverCmd(Player& player, DiscoveryType discovery, CmdPtr pNext) : 
	Cmd(player), m_discovery(discovery), m_pNext(std::move(pNext))
{
}

void DiscoverCmd::UpdateClient(const Controller& controller) const
{
	Output::ChooseExploreDiscovery msg;
	controller.SendMessage(msg, m_player);
}

CmdPtr DiscoverCmd::Process(const Input::CmdMessage& msg, const Controller& controller)
{
	auto& m = CastThrow<const Input::CmdExploreDiscovery>(msg);

	return std::move(m_pNext);
}

void DiscoverCmd::Undo(const Controller& controller)
{
}
