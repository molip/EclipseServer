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
	InfluenceCmd(Player& player, int iPhase = 0);

	virtual void UpdateClient(const Controller& controller) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller) override;
	virtual bool IsAction() const override { return true; } 

private:
	std::vector<MapPos> m_srcs;
	int m_iPhase;
};

class InfluenceDstCmd : public Cmd
{
public:
	InfluenceDstCmd(Player& player, const MapPos* pSrcPos, int iPhase = 0);

	virtual void UpdateClient(const Controller& controller) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller) override;
	virtual void Undo(const Controller& controller) override;
	virtual bool CanUndo() const override { return m_discovery == DiscoveryType::None; }

private:
	std::unique_ptr<MapPos> m_pSrcPos;
	MapPos* m_pDstPos;
	std::vector<MapPos> m_dsts;
	DiscoveryType m_discovery;

	int m_iPhase;
};