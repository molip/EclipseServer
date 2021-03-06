#pragma once

#include "PhaseCmd.h"
#include "MapPos.h"

#include <vector> 

class Hex;
enum class DiscoveryType;

class InfluenceCmd : public PhaseCmd
{
public:
	InfluenceCmd() : m_flipsLeft(0)  {}
	InfluenceCmd(Colour colour, const LiveGame& game, int iPhase = 0, int flipsLeft = 2);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;
	virtual bool IsAction() const override { return true; } 
	virtual std::string GetActionName() const override { return "Influence"; }

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	std::vector<MapPos> GetSources(const LiveGame& game) const;
	int GetMaxFlips(const LiveGame& game) const;

	int m_flipsLeft;
};

class InfluenceDstCmd : public PhaseCmd
{
public:
	InfluenceDstCmd();
	InfluenceDstCmd(Colour colour, const LiveGame& game, const MapPos* pSrcPos, int iPhase, int flipsLeft);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;
	virtual bool CanUndo() const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	std::vector<MapPos> GetDests(const LiveGame& game) const;
	int GetMaxFlips(const LiveGame& game) const;

	std::unique_ptr<MapPos> m_pSrcPos;
	DiscoveryType m_discovery;
	int m_flipsLeft;
};