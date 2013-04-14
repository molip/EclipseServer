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

ExploreCmd::ExploreCmd(Player& player) : 
	m_player(player), m_stage(Stage::Pos)
{
	m_phases.push_back(PhasePtr(new Phase));
	GetPossiblePositions();
}

void ExploreCmd::AcceptMessage(const Input::CmdMessage& msg)
{
	Game* pGame = m_player.GetCurrentGame();
	AssertThrow(!!pGame);
	Team& team = pGame->GetTeam(m_player);
	
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

			GetHexChoices(*pGame);

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
				
				Hex& hex = pGame->GetMap().AddHex(phase.m_pos, phase.m_hexChoices[phase.m_iHex].m_idHex, hc.m_rotations[phase.m_iRot]);
				
				if (phase.m_bInfluence = m.m_bInfluence)
					hex.SetOwner(&team);

				// TODO: Something better.
				Controller::Get()->SendMessage(Output::UpdateMap(*pGame), *pGame);

				phase.m_discovery = hex.GetDiscoveryTile();
				if (!hex.GetShips().empty())
					phase.m_discovery = DiscoveryType::None;

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
	if ((int)m_phases.size() < Race(m_player.GetCurrentTeam()->GetRace()).GetExploreRate())
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

		Game* pGame = m_player.GetCurrentGame();
		AssertThrow(!!pGame);

		m_phases.pop_back();

		const Phase& phase = GetPhase();
		if (phase.m_discovery == DiscoveryType::None)
		{
			m_stage = Stage::Hex;
			pGame->GetMap().DeleteHex(m_phases.back()->m_pos);

			// TODO: Something better.
			Controller::Get()->SendMessage(Output::UpdateMap(*pGame), *pGame);
		}
		else
			m_stage = Stage::Discovery;
	}

	return false;
}

void ExploreCmd::GetPossiblePositions()
{
	const Game* pGame = m_player.GetCurrentGame();
	AssertThrow(!!pGame);
	const Team& team = pGame->GetTeam(m_player);

	auto& positions = GetPhase().m_positions;

	const Map::HexMap& hexes = pGame->GetMap().GetHexes();
	for (auto& h : hexes)
		if (h.second->GetOwner() == &team) // TODO: Check ships.
			pGame->GetMap().GetEmptyNeighbours(h.first, team.HasTech(TechType::WormholeGen), positions);
}

void ExploreCmd::GetHexChoices(Game& game)
{
	Phase& phase = GetPhase();
	const Team& team = game.GetTeam(m_player);
	bool bWormholeGen = team.HasTech(TechType::WormholeGen);

	std::vector<const Hex*> hexes = game.GetMap().GetSurroundingHexes(phase.m_pos, team);

	HexRing ring = phase.m_pos.GetRing();
	HexBag& bag = game.GetHexBag(ring);
	Race race(team.GetRace());

	phase.m_hexChoices.clear();
	
	for (int i = 0; i < race.GetExploreChoices(); ++i)
	{
		int id = bag.TakeTile();
		Hex hex(nullptr, id, 0);

		HexChoice hc(id, hex.GetShips().empty() || race.IsAncientsAlly());
		
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