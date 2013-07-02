#include "stdafx.h"
#include "DiscoverCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "Player.h"
#include "Race.h"
#include "Team.h"
#include "LiveGame.h"

DiscoverCmd::DiscoverCmd(Colour colour, LiveGame& game, DiscoveryType discovery) : 
	Cmd(colour), m_discovery(discovery)
{
}

void DiscoverCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	Output::ChooseDiscovery msg(game.CanRemoveCmd());
	controller.SendMessage(msg, GetPlayer(game));
}

CmdPtr DiscoverCmd::Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game)
{
	auto& m = CastThrow<const Input::CmdExploreDiscovery>(msg);

	return GetNextCmd(game);
}

void DiscoverCmd::Undo(const Controller& controller, LiveGame& game)
{
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
