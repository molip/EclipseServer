#pragma once

#include "Cmd.h"
#include "Discovery.h"

#include <vector> 
#include <set> 

enum class TechType;

class DiscoverCmd : public Cmd
{
public:
	DiscoverCmd() : m_discovery(DiscoveryType::None), m_idHex(0) {}
	DiscoverCmd(Colour colour, const LiveGame& game, DiscoveryType discovery, int idHex);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	bool CanKeep() const;
	bool CanUse(const LiveGame& game) const;
	std::vector<TechType> GetAncientTechs(const LiveGame& game) const;

	DiscoveryType m_discovery;
	int m_idHex;
};
