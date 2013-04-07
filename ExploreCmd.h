#pragma once

#include "Cmd.h"

#include <vector> 

class Player;

class ExploreCmd : public Cmd
{
public:
	ExploreCmd(Player& player);

	virtual void AcceptMessage(const Input::CmdMessage& msg) override;
	virtual void UpdateClient(const Controller& controller) const override;
	virtual bool IsFinished() const override;
	virtual bool CanUndo() override;
	virtual void Undo() override;

private:
	enum class Stage { Pos, Hex, Influence, Discovery, Finished };

	Stage m_stage;
	Player& m_player;

	// Stage::Pos
	int m_x, m_y;
	std::vector<int> m_hexes;

	// Stage::Hex
	int m_rotation;
	int m_iHex;
	bool m_bReject;

	// Stage::Influence
	//bool m_bInfluence;

	// Stage::Discovery
	//std::unique_ptr<DiscoveryChoice> m_pDiscoveryChoice;
};