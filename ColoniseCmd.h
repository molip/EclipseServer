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
	ColoniseCmd(Player& player);

	virtual void UpdateClient(const Controller& controller) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller) override;

private:
	std::vector<MapPos> m_positions;
};

class ColoniseSquaresCmd : public Cmd
{
public:
	ColoniseSquaresCmd(Player& player, const MapPos& pos);

	virtual void UpdateClient(const Controller& controller) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller) override;
	virtual void Undo(const Controller& controller) override;
	virtual bool HasRecord() const { return true; } 

private:
	void Init(const MapPos& pos);

	MapPos m_pos;
	std::vector<SquareType> m_squares;
	int m_squareCounts[SquareType::_Count];
};
