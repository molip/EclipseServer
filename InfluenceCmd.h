#pragma once

#include "PhaseCmd.h"
#include "MapPos.h"
#include "Discovery.h"

#include <vector> 
#include <set> 

class Hex;

class InfluenceCmd : public PhaseCmd
{
public:
	InfluenceCmd() {}
	InfluenceCmd(Colour colour, const LiveGame& game, int iPhase = 0);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, CommitSession& session) override;
	virtual bool IsAction() const override { return true; } 
	virtual std::string GetActionName() const override { return "Inlfuence"; }

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	std::vector<MapPos> GetSources(const LiveGame& game) const;
};

class InfluenceDstCmd : public PhaseCmd
{
public:
	InfluenceDstCmd() : m_discovery(DiscoveryType::None) {}
	InfluenceDstCmd(Colour colour, const LiveGame& game, const MapPos* pSrcPos, int iPhase = 0);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, CommitSession& session) override;
	virtual bool CanUndo() const override { return m_discovery == DiscoveryType::None; }

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	std::vector<MapPos> GetDests(const LiveGame& game) const;

	std::unique_ptr<MapPos> m_pSrcPos;
	DiscoveryType m_discovery;
};