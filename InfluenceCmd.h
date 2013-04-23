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

class InfluenceCmd : public Cmd
{
public:
	InfluenceCmd(Game& game, Player& player);

	virtual void AcceptMessage(const Input::CmdMessage& msg) override;
	virtual void UpdateClient(const Controller& controller) const override;
	virtual bool IsFinished() const override;
	virtual bool CanUndo() const override;
	virtual bool Undo() override;

private:
	struct Phase
	{
		Phase() : m_iSrc(-1), m_iDst(-1), m_bReject(false), m_discovery(DiscoveryType::None) {}
		
		std::vector<MapPos> m_srcs, m_dsts;
		bool m_bReject;
		int m_iSrc, m_iDst; // -1: influence track.
		DiscoveryType m_discovery;
	};

	enum class Stage { Src, Dst, Discovery, Finished };

	void GetSrcChoices();
	void GetDstChoices();
	Hex* TransferDisc(const MapPos* pSrcPos, const MapPos* pDstPos);
		
	Phase& GetPhase() { return *m_phases.back(); }
	const Phase& GetPhase() const { return *m_phases.back(); }
	void EndPhase();

	Stage m_stage;
	Player& m_player;
	Game& m_game;
	Team& m_team;

	typedef std::unique_ptr<Phase> PhasePtr;
	std::vector<PhasePtr> m_phases;
};