#pragma once

#include "PhaseCmd.h"
#include "MapPos.h"

#include <vector>

enum class ShipType;
class Hex;

class MoveCmd : public PhaseCmd
{
public:
	MoveCmd() {}
	MoveCmd(Colour colour, const LiveGame& game, int iPhase = 0);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;
	virtual bool IsAction() const { return m_iPhase == 0; } 
	virtual std::string GetActionName() const override { return "Move"; }

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	bool CanMoveFrom(const Hex& hex, const LiveGame& game) const;
};

class MoveDstCmd : public PhaseCmd
{
public:
	MoveDstCmd() {}
	MoveDstCmd(Colour colour, const LiveGame& game, const MapPos& src, ShipType ship, int iPhase);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	std::set<MapPos> GetDsts(const LiveGame& game) const;
	void AddNeighbourDsts(const LiveGame& game, const MapPos& pos, int movement, std::set<MapPos>& dsts) const;

	MapPos m_src;
	ShipType m_ship;
};

