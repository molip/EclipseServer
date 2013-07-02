#pragma once

#include "Cmd.h"
#include "MapPos.h"
#include "Discovery.h"

#include <vector> 
#include <set> 

class Hex;

class InfluenceCmd : public Cmd
{
public:
	InfluenceCmd() : m_iPhase(-1) {}
	InfluenceCmd(Colour colour, LiveGame& game, int iPhase = 0);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game) override;
	virtual bool IsAction() const override { return true; } 

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	std::vector<MapPos> m_srcs;
	int m_iPhase;
};

class InfluenceDstCmd : public Cmd
{
public:
	InfluenceDstCmd() : m_iPhase(-1), m_discovery(DiscoveryType::None) {}
	InfluenceDstCmd(Colour colour, LiveGame& game, const MapPos* pSrcPos, int iPhase = 0);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game) override;
	virtual void Undo(const Controller& controller, LiveGame& game) override;
	virtual bool CanUndo() const override { return m_discovery == DiscoveryType::None; }
	virtual bool HasRecord() const { return true; } 

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	std::unique_ptr<MapPos> m_pSrcPos;
	std::vector<MapPos> m_dsts;
	DiscoveryType m_discovery;

	int m_iPhase;
};