#pragma once

#include "PhaseCmd.h"
#include "MapPos.h"
#include "Discovery.h"

#include <vector> 
#include <set> 

class Player;
class Map;
class Team;
class Game;

class ExploreCmd : public PhaseCmd
{
public:
	ExploreCmd() : m_idHex(-1), m_iPos(-1) {}
	ExploreCmd(Colour colour, LiveGame& game, int iPhase = 0);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game) override;
	virtual bool IsAction() const override { return true; } 
	virtual bool CanUndo() const override { return m_idHex < 0; }

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	std::vector<MapPos> m_positions;
	int m_idHex, m_iPos;
};

class ExploreHexCmd : public PhaseCmd
{
public:
	ExploreHexCmd() : m_iRot(-1), m_iHex(-1), m_bInfluence(false), m_idTaken(-1), m_discovery(DiscoveryType::None) {}

	ExploreHexCmd(Colour colour, LiveGame& game, const MapPos& pos, std::vector<int> hexIDs, int iPhase);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game) override;
	virtual void Undo(const Controller& controller, LiveGame& game) override;
	virtual bool CanUndo() const override { return m_idTaken < 0 && m_discovery == DiscoveryType::None; }
	virtual bool HasRecord() const { return m_iHex >= 0; } 

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	struct HexChoice
	{
		HexChoice(int idHex = 0, bool bCanInfluence = false) : m_idHex(idHex), m_bCanInfluence(bCanInfluence) {}
		int m_idHex;
		bool m_bCanInfluence;
		std::vector<int> m_rotations;

		void Save(Serial::SaveNode& node) const;
		void Load(const Serial::LoadNode& node);
	};

	std::vector<int> m_hexIDs;
	std::vector<HexChoice> m_hexChoices;
	int m_iRot;
	int m_iHex;
	bool m_bInfluence;
	MapPos m_pos;
	int m_idTaken;
	DiscoveryType m_discovery;
};
