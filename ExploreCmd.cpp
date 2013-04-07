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
	m_player(player), m_stage(Stage::Pos), m_x(0), m_y(0), m_rotation(0), m_iHex(0), m_bReject(false)
{
}

void ExploreCmd::AcceptMessage(const Input::CmdMessage& msg)
{
	Game* pGame = m_player.GetCurrentGame();
	AssertThrow(!!pGame);
	
	switch (m_stage)
	{
		case Stage::Pos:
		{
			auto& m = CastMessage<const Input::CmdExplorePos>(msg);
			
			AssertThrow("ExploreCmd::AcceptMessage (Stage::Pos)", m_hexes.empty());
			
			m_x = m.m_x;
			m_y = m.m_y;

			HexRing ring = MapPos(m_x, m_y).GetRing();
			HexBag& bag = pGame->GetHexBag(ring);
			
			for (int i = 0; i < Race(m_player.GetCurrentTeam()->GetRace()).GetExploreChoices(); ++i)
				m_hexes.push_back(bag.TakeTile());

			m_stage = Stage::Hex;
			break;
		}
		case Stage::Hex:
		{
			if (dynamic_cast<const Input::CmdExploreReject*>(&msg))
			{
				m_bReject = true;
				m_stage = Stage::Finished;
			}
			
			if (m_hexes.size() > 1 || m_bReject)
			{
				// TODO: Add discarded hexes to discard pile. 
			}
			
			if (!m_bReject)
			{
				auto& m = CastMessage<const Input::CmdExploreHex>(msg);
				AssertThrowXML("ExploreCmd::AcceptMessage (Stage::Hex): invalid rotation", m.m_rotation >= 0 && m.m_rotation < 6);
				m_rotation = m.m_rotation;
				
				AssertThrowXML("ExploreCmd::AcceptMessage (Stage::Hex): hex index", m.m_iHex >= 0 && m.m_iHex < (int)m_hexes.size());
				m_iHex = m.m_iHex;

				pGame->GetMap().AddHex(MapPos(m_x, m_y), m_hexes[m_iHex], m_rotation);

				// TODO: Something better.
				Controller::Get()->SendMessage(Output::UpdateMap(*pGame), *pGame);

				m_stage = Stage::Finished;
			}
			break;
		}
	}
}

void ExploreCmd::UpdateClient(const Controller& controller) const
{
	switch (m_stage)
	{
	case Stage::Pos:
		controller.SendMessage(Output::ChooseExplorePos(), m_player);
		break;
	case Stage::Hex:
		controller.SendMessage(Output::ChooseExploreHex(m_x, m_y, m_hexes), m_player);
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