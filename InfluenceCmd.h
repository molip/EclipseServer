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
	InfluenceCmd(Colour colour, LiveGame& game, int iPhase = 0);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game) override;
	virtual bool IsAction() const override { return true; } 

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	std::vector<MapPos> m_srcs;
};

class InfluenceDstCmd : public PhaseCmd
{
public:
	InfluenceDstCmd() : m_discovery(DiscoveryType::None) {}
	InfluenceDstCmd(Colour colour, LiveGame& game, const MapPos* pSrcPos, int iPhase = 0);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game) override;
	virtual bool CanUndo() const override { return m_discovery == DiscoveryType::None; }
	virtual bool HasRecord() const { return true; } 

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	std::unique_ptr<MapPos> m_pSrcPos;
	std::vector<MapPos> m_dsts;
	DiscoveryType m_discovery;
};