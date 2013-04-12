#pragma once

#include "Cmd.h"
#include "MapPos.h"

#include <vector> 
#include <set> 

class Player;
class Map;

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
	struct HexChoice
	{
		HexChoice(int idHex = 0, bool bCanInfluence = false) : m_idHex(idHex), m_bCanInfluence(bCanInfluence) {}
		int m_idHex;
		bool m_bCanInfluence;
		std::vector<int> m_rotations;
	};
	struct Phase
	{
		Phase() : m_iRot(0), m_iHex(0), m_bReject(false) {}
		
		// Stage::Pos
		std::set<MapPos> m_positions;
		MapPos m_pos;
		std::vector<HexChoice> m_hexChoices;

		// Stage::Hex
		int m_iRot;
		int m_iHex;
		bool m_bReject;

		// Stage::Influence
		//bool m_bInfluence;

		// Stage::Discovery
		//std::unique_ptr<DiscoveryChoice> m_pDiscoveryChoice;
	};

	enum class Stage { Pos, Hex, Influence, Discovery, Finished };

	void GetPossiblePositions();
	void GetHexChoices(Game& game);
		
	Phase& GetPhase() { return *m_phases.back(); }
	const Phase& GetPhase() const { return *m_phases.back(); }
	void EndPhase();

	Stage m_stage;
	Player& m_player;

	typedef std::unique_ptr<Phase> PhasePtr;
	std::vector<PhasePtr> m_phases;
};