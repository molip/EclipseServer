#pragma once

#include "Cmd.h"
#include "MapPos.h"
#include "Discovery.h"

#include <vector> 
#include <set> 

class Player;
class Map;
class Team;
class Game;

class ExploreCmd : public Cmd
{
public:
	ExploreCmd(Player& player, int iPhase = 0);

	virtual void UpdateClient(const Controller& controller) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller) override;
	virtual bool IsAction() const override { return true; } 
	virtual bool CanUndo() const override { return m_idHex < 0; }

private:
	std::vector<MapPos> m_positions;
	int m_idHex, m_iPos;
	int m_iPhase;
};

class ExploreHexCmd : public Cmd
{
public:
	ExploreHexCmd(Player& player, const MapPos& pos, std::vector<int> hexIDs, int iPhase);

	virtual void UpdateClient(const Controller& controller) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller) override;
	virtual void Undo(const Controller& controller) override;
	virtual bool CanUndo() const override { return m_idTaken < 0 && m_discovery == DiscoveryType::None; }
	virtual bool HasRecord() const { return m_iHex >= 0; } 

private:
	struct HexChoice
	{
		HexChoice(int idHex = 0, bool bCanInfluence = false) : m_idHex(idHex), m_bCanInfluence(bCanInfluence) {}
		int m_idHex;
		bool m_bCanInfluence;
		std::vector<int> m_rotations;
	};

	std::vector<int> m_hexIDs;
	std::vector<HexChoice> m_hexChoices;
	int m_iRot;
	int m_iHex;
	bool m_bInfluence;
	const MapPos& m_pos;
	int m_iPhase;
	int m_idTaken;
	DiscoveryType m_discovery;
};
