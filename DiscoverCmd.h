#pragma once

#include "Cmd.h"
#include "Discovery.h"

#include <vector> 
#include <set> 

class DiscoverCmd : public Cmd
{
public:
	DiscoverCmd(Player& player, DiscoveryType discovery);

	virtual void UpdateClient(const Controller& controller) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller) override;
	virtual void Undo(const Controller& controller) override;
	virtual bool HasRecord() const { return true; } 

private:
	virtual CmdPtr GetNextCmd() const { return nullptr; }

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
