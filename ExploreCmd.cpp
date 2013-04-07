#include "stdafx.h"
#include "ExploreCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "Player.h"
#include "Race.h"
#include "Game.h"

template <typename T>
T& CastMessage(const Input::CmdMessage& msg)
{
	T* pMsg = dynamic_cast<T*>(&msg);
	AssertThrow(std::string("ExploreCmd::AcceptMessage: Expected ") + typeid(T).name() + ", got " + typeid(msg).name(), !!pMsg);
	return *pMsg;
}

ExploreCmd::ExploreCmd(Player& player) : 
	m_player(player), m_stage(Stage::Pos)
{
	m_phases.push_back(PhasePtr(new Phase));
}

void ExploreCmd::AcceptMessage(const Input::CmdMessage& msg)
{
	Game* pGame = m_player.GetCurrentGame();
	AssertThrow(!!pGame);
	
	Phase& phase = GetPhase();

	switch (m_stage)
	{
		case Stage::Pos:
		{
			if (dynamic_cast<const Input::CmdExploreReject*>(&msg))
			{
				phase.m_bReject = true;
				m_stage = Stage::Finished;
				return;
			}

			auto& m = CastMessage<const Input::CmdExplorePos>(msg);
			
			AssertThrow("ExploreCmd::AcceptMessage (Stage::Pos)", phase.m_hexes.empty());
			
			phase.m_x = m.m_x;
			phase.m_y = m.m_y;

			HexRing ring = MapPos(phase.m_x, phase.m_y).GetRing();
			HexBag& bag = pGame->GetHexBag(ring);
			
			for (int i = 0; i < Race(m_player.GetCurrentTeam()->GetRace()).GetExploreChoices(); ++i)
				phase.m_hexes.push_back(bag.TakeTile());

			m_stage = Stage::Hex;
			break;
		}
		case Stage::Hex:
		{
			if (dynamic_cast<const Input::CmdExploreReject*>(&msg))
			{
				phase.m_bReject = true;
				EndPhase();
			}
			
			if (phase.m_hexes.size() > 1 || phase.m_bReject)
			{
				// TODO: Add discarded hexes to discard pile. 
			}
			
			if (!phase.m_bReject)
			{
				auto& m = CastMessage<const Input::CmdExploreHex>(msg);
				AssertThrowXML("ExploreCmd::AcceptMessage (Stage::Hex): invalid rotation", m.m_rotation >= 0 && m.m_rotation < 6);
				phase.m_rotation = m.m_rotation;
				
				AssertThrowXML("ExploreCmd::AcceptMessage (Stage::Hex): hex index", m.m_iHex >= 0 && m.m_iHex < (int)phase.m_hexes.size());
				phase.m_iHex = m.m_iHex;

				pGame->GetMap().AddHex(MapPos(phase.m_x, phase.m_y), phase.m_hexes[phase.m_iHex], phase.m_rotation);

				// TODO: Something better.
				Controller::Get()->SendMessage(Output::UpdateMap(*pGame), *pGame);

				EndPhase();
			}
			break;
		}
	}
}

void ExploreCmd::EndPhase()
{
	if ((int)m_phases.size() < Race(m_player.GetCurrentTeam()->GetRace()).GetExploreRate())
	{
		m_stage = Stage::Pos;
		m_phases.push_back(PhasePtr(new Phase));
	}
	else
		m_stage = Stage::Finished;
}

void ExploreCmd::UpdateClient(const Controller& controller) const
{
	const Phase& phase = GetPhase();

	switch (m_stage)
	{
	case Stage::Pos:
		controller.SendMessage(Output::ChooseExplorePos(m_phases.size() > 1), m_player);
		break;
	case Stage::Hex:
		controller.SendMessage(Output::ChooseExploreHex(phase.m_x, phase.m_y, phase.m_hexes), m_player);
		break;
	}
}

bool ExploreCmd::IsFinished() const 
{
	return m_stage == Stage::Finished;
}

bool ExploreCmd::CanUndo()
{
	return m_stage != Stage::Hex;
}

void ExploreCmd::Undo()
{
}
