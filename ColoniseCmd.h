#pragma once

#include "Cmd.h"
#include "MapPos.h"
#include "Discovery.h"
#include "Hex.h"

#include <vector> 
#include <set> 

enum class Resource;

class ColoniseCmd : public Cmd
{
public:
	ColoniseCmd() {}
	ColoniseCmd(Colour colour, const LiveGame& game);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	std::vector<MapPos> m_positions;
};

class ColoniseSquaresCmd : public Cmd
{
public:
	ColoniseSquaresCmd() {}
	ColoniseSquaresCmd(Colour colour, const LiveGame& game, const MapPos& pos);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	MapPos m_pos;
	EnumArray<SquareType, int> m_squareCounts;
};
