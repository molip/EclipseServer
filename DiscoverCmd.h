#pragma once

#include "Cmd.h"
#include "Discovery.h"

#include <vector> 
#include <set> 

class DiscoverCmd : public Cmd
{
public:
	DiscoverCmd() : m_discovery(DiscoveryType::None) {}
	DiscoverCmd(Colour colour, const LiveGame& game, DiscoveryType discovery);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, CommitSession& session) override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	virtual CmdPtr GetNextCmd(const LiveGame& game) const { return nullptr; }

	DiscoveryType m_discovery;
		//std::unique_ptr<DiscoveryChoice> m_pDiscoveryChoice;
		
		//Take victory points
		//Take discovery
		//	Tech 
		//		choose tech
		//	Ship part: 
		//		Choose slot
		//		Save for later
};
