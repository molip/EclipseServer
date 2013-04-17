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
	ExploreCmd(Game& game, Player& player);

	virtual void AcceptMessage(const Input::CmdMessage& msg) override;
	virtual void UpdateClient(const Controller& controller) const override;
	virtual bool IsFinished() const override;
	virtual bool CanUndo() override;
	virtual bool Undo() override;

private:
	struct HexChoice
	{
		HexChoice(int idHex = 0, bool bCanInfluence = false) : m_idHex(idHex), m_bCanInfluence(bCanInfluence) {}
		int m_idHex;
		bool m_bCanInfluence;
		std::vector<int> m_rotations;
	};
	struct Phase
	{
		Phase() : m_iRot(0), m_iHex(0), m_bReject(false), m_bInfluence(false), m_discovery(DiscoveryType::None) {}
		
		// Stage::Pos
		std::set<MapPos> m_positions;
		MapPos m_pos;
		std::vector<HexChoice> m_hexChoices;

		// Stage::Hex
		int m_iRot;
		int m_iHex;
		bool m_bReject;
		bool m_bInfluence;
		
		// Stage::Discovery
		DiscoveryType m_discovery;
		//std::unique_ptr<DiscoveryChoice> m_pDiscoveryChoice;
		
		//Take victory points
		//Take discovery
		//	Tech 
		//		choose tech
		//	Ship part: 
		//		Choose slot
		//		Save for later
	};

	enum class Stage { Pos, Hex, Discovery, Finished };

	void GetPossiblePositions();
	void GetHexChoices();
		
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