#include "stdafx.h"
#include "ExploreCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "Player.h"
#include "Race.h"
#include "Game.h"
#include "Map.h"

template <typename T>
T& CastMessage(const Input::CmdMessage& msg)
{
	T* pMsg = dynamic_cast<T*>(&msg);
	AssertThrow(std::string("ExploreCmd::AcceptMessage: Expected ") + typeid(T).name() + ", got " + typeid(msg).name(), !!pMsg);
	return *pMsg;
}

ExploreCmd::ExploreCmd(Game& game, Player& player) : 
	m_player(player), m_stage(Stage::Pos), m_game(game), m_team(game.GetTeam(player))
{
	m_phases.push_back(PhasePtr(new Phase));
	GetPossiblePositions();
}

void ExploreCmd::AcceptMessage(const Input::CmdMessage& msg)
{
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
			
			AssertThrow("ExploreCmd::AcceptMessage (Stage::Pos): hexes already taken", phase.m_hexChoices.empty());
			AssertThrow("ExploreCmd::AcceptMessage (Stage::Pos): invalid pos index", m.m_iPos >= 0 && m.m_iPos < (int)phase.m_positions.size());
			
			auto it = phase.m_positions.begin();
			for (int i = 0; i < m.m_iPos; ++i)	
				++it;

			phase.m_pos = *it;

			GetHexChoices();

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
			
			if (phase.m_hexChoices.size() > 1 || phase.m_bReject)
			{
				// TODO: Add discarded hexes to discard pile. 
			}
			
			if (!phase.m_bReject)
			{
				auto& m = CastMessage<const Input::CmdExploreHex>(msg);

				AssertThrowXML("ExploreCmd::AcceptMessage (Stage::Hex): hex index", m.m_iHex >= 0 && m.m_iHex < (int)phase.m_hexChoices.size());
				phase.m_iHex = m.m_iHex;

				auto& hc = phase.m_hexChoices[phase.m_iHex];

				AssertThrowXML("ExploreCmd::AcceptMessage (Stage::Hex): rotation index", m.m_iRot >= 0 && m.m_iRot < (int)hc.m_rotations.size());
				phase.m_iRot = m.m_iRot;
				
				Controller* pCtrlr = Controller::Get(); // TODO: Something better.

				// TODO: exception safety
				Hex& hex = m_game.GetMap().AddHex(phase.m_pos, phase.m_hexChoices[phase.m_iHex].m_idHex, hc.m_rotations[phase.m_iRot]);
				
				if (phase.m_bInfluence = m.m_bInfluence)
				{
					m_team.GetInfluenceTrack().RemoveDiscs(1);
					hex.SetOwner(&m_team);
					pCtrlr->SendMessage(Output::UpdateInfluenceTrack(m_team), m_game);
					phase.m_discovery = hex.GetDiscoveryTile();
				}					

				pCtrlr->SendMessage(Output::UpdateMap(m_game), m_game);

				if (phase.m_discovery == DiscoveryType::None)
					EndPhase();
				else
					m_stage = Stage::Discovery;
			}
			break;
		}
		case Stage::Discovery:
		{
			auto& m = CastMessage<const Input::CmdExploreDiscovery>(msg);
			EndPhase();
			break;
		}
	}
}

void ExploreCmd::EndPhase()
{
	if ((int)m_phases.size() < Race(m_team.GetRace()).GetExploreRate())
	{
		m_stage = Stage::Pos;
		m_phases.push_back(PhasePtr(new Phase));
		GetPossiblePositions();
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
		controller.SendMessage(Output::ChooseExplorePos(phase.m_positions, m_phases.size() > 1), m_player);
		break;
	case Stage::Hex:
		{
			Output::ChooseExploreHex msg(phase.m_pos.GetX(), phase.m_pos.GetY());
			for (auto& hc : phase.m_hexChoices)
				msg.AddHexChoice(hc.m_idHex, hc.m_rotations, hc.m_bCanInfluence);

			controller.SendMessage(msg, m_player);
			break;
		}
	case Stage::Discovery:
		{
			Output::ChooseExploreDiscovery msg;
			controller.SendMessage(msg, m_player);
			break;
		}
	}
}

bool ExploreCmd::IsFinished() const 
{
	return m_stage == Stage::Finished;
}

bool ExploreCmd::CanUndo()
{
	return m_stage != Stage::Hex && m_stage != Stage::Discovery; // Hex/tile has been revealed.
}

bool ExploreCmd::Undo()
{
	AssertThrow("ExploreCmd::Undo", CanUndo()); 
	
	if (m_stage == Stage::Pos) 
	{
		if (m_phases.size() == 1)
			return true; 

		m_phases.pop_back();

		const Phase& phase = GetPhase();
		if (phase.m_discovery == DiscoveryType::None)
		{
			m_stage = Stage::Hex;

			Controller* pCtrlr = Controller::Get(); // TODO: Something better.
			
			if (!phase.m_bReject)
			{
				pCtrlr->SendMessage(Output::UpdateMap(m_game), m_game);
				m_game.GetMap().DeleteHex(m_phases.back()->m_pos);
			}

			if (phase.m_bInfluence)
			{
				m_team.GetInfluenceTrack().AddDiscs(1);
				pCtrlr->SendMessage(Output::UpdateInfluenceTrack(m_team), m_game);
			}					
		}
		else
			m_stage = Stage::Discovery;
	}

	return false;
}

void ExploreCmd::GetPossiblePositions()
{
	auto& positions = GetPhase().m_positions;

	const Map::HexMap& hexes = m_game.GetMap().GetHexes();
	for (auto& h : hexes)
		if (h.second->GetOwner() == &m_team) // TODO: Check ships.
			m_game.GetMap().GetEmptyNeighbours(h.first, m_team.HasTech(TechType::WormholeGen), positions);
}

void ExploreCmd::GetHexChoices()
{
	Phase& phase = GetPhase();
	bool bWormholeGen = m_team.HasTech(TechType::WormholeGen);

	std::vector<const Hex*> hexes = m_game.GetMap().GetSurroundingHexes(phase.m_pos, m_team);

	HexRing ring = phase.m_pos.GetRing();
	HexBag& bag = m_game.GetHexBag(ring);
	Race race(m_team.GetRace());

	phase.m_hexChoices.clear();
	
	for (int i = 0; i < race.GetExploreChoices(); ++i)
	{
		int id = bag.TakeTile();
		Hex hex(nullptr, id, 0);

		bool bCanInfluenceHex = hex.GetShips().empty() || race.IsAncientsAlly();
		HexChoice hc(id, bCanInfluenceHex && m_team.GetInfluenceTrack().GetDiscCount() > 0);
		
		EdgeSet inner = hex.GetWormholes();

		for (int i = 0; i < 6; ++i) // Try each rotation.
		{
			bool bMatch = false;
			for (auto e : EnumRange<Edge>())
			{
				const Hex* pHex = hexes[(int)e];
				
				bool bInner = inner[RotateEdge(e, -i)];
				bool bOuter = pHex && pHex->HasWormhole(ReverseEdge(e));

				if (bWormholeGen)
					bMatch = pHex && (bOuter || bInner);
				else
					bMatch = bOuter && bInner;

				if (bMatch)
					break;
			}
			if (bMatch)
				hc.m_rotations.push_back(i);
		}
		phase.m_hexChoices.push_back(hc);
	}
}