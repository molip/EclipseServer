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
	ExploreCmd() {}
	ExploreCmd(Colour colour, const LiveGame& game, int iPhase = 0);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;
	virtual bool IsAction() const override { return true; } 
	virtual std::string GetActionName() const override { return "Explore"; }
	virtual bool CanUndo() const override { return GetRecordCount() == 0; } // Can undo abort, but not hex reveal.

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	std::vector<MapPos> GetPositions(const LiveGame& game) const;
};

class ExploreHexCmd : public PhaseCmd
{
public:
	ExploreHexCmd() : m_iRot(-1), m_iHex(-1), m_bInfluence(false), m_bTaken(false), m_discovery(DiscoveryType::None) {}

	ExploreHexCmd(Colour colour, const LiveGame& game, const MapPos& pos, std::vector<int> hexIDs, int iPhase);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;
	virtual bool CanUndo() const override { return !m_bTaken && m_discovery == DiscoveryType::None; }

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

	std::vector<HexChoice> GetHexChoices(const LiveGame& game) const;

	std::vector<int> m_hexIDs;
	int m_iRot;
	int m_iHex;
	bool m_bInfluence;
	MapPos m_pos;
	bool m_bTaken;
	DiscoveryType m_discovery;
};
