#include "stdafx.h"
#include "InfluenceCmd.h"
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
	AssertThrow(std::string("InfluenceCmd::AcceptMessage: Expected ") + typeid(T).name() + ", got " + typeid(msg).name(), !!pMsg);
	return *pMsg;
}

InfluenceCmd::InfluenceCmd(Game& game, Player& player) : 
	m_player(player), m_stage(Stage::Src), m_game(game), m_team(game.GetTeam(player))
{
	m_phases.push_back(PhasePtr(new Phase));
	GetSrcChoices();
}

void InfluenceCmd::AcceptMessage(const Input::CmdMessage& msg)
{
	Phase& phase = GetPhase();

	switch (m_stage)
	{
		case Stage::Src:
		{
			if (dynamic_cast<const Input::CmdExploreReject*>(&msg))
			{
				phase.m_bReject = true;
				m_stage = Stage::Finished;
				return;
			}
			auto& m = CastMessage<const Input::CmdInfluenceSrc>(msg);
			AssertThrow("InfluenceCmd::AcceptMessage (Stage::Src): invalid pos index", m.m_iPos >= -1 && m.m_iPos < (int)phase.m_srcs.size());
			phase.m_iSrc = m.m_iPos;
			GetDstChoices();
			m_stage = Stage::Dst;
			break;
		}
		case Stage::Dst:
		{
			auto& m = CastMessage<const Input::CmdInfluenceDst>(msg);
			AssertThrow("InfluenceCmd::AcceptMessage (Stage::Dst): invalid pos index", m.m_iPos >= -1 && m.m_iPos < (int)phase.m_dsts.size());
			phase.m_iDst = m.m_iPos;

			Hex* pDstHex = TransferDisc(phase.m_iSrc < 0 ? nullptr : &phase.m_srcs[phase.m_iSrc], 
										phase.m_iDst < 0 ? nullptr : &phase.m_dsts[phase.m_iDst]);

			Controller* pCtrlr = Controller::Get(); // TODO: Something better.

			pCtrlr->SendMessage(Output::UpdateMap(m_game), m_game);
			if (phase.m_iSrc < 0 || phase.m_iDst < 0)
				pCtrlr->SendMessage(Output::UpdateInfluenceTrack(m_team), m_game);
			
			phase.m_discovery = pDstHex ? pDstHex->GetDiscoveryTile() : DiscoveryType::None;

			if (phase.m_discovery == DiscoveryType::None)
				EndPhase();
			else
				m_stage = Stage::Discovery;

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

void InfluenceCmd::EndPhase()
{
	if (m_phases.size() < 2)
	{
		m_stage = Stage::Src;
		m_phases.push_back(PhasePtr(new Phase));
		GetSrcChoices();
	}
	else
		m_stage = Stage::Finished;
}

void InfluenceCmd::UpdateClient(const Controller& controller) const
{
	const Phase& phase = GetPhase();

	switch (m_stage)
	{
		case Stage::Src:
			controller.SendMessage(Output::ChooseInfluenceSrc(phase.m_srcs, m_team.GetInfluenceTrack().GetDiscCount() > 0), m_player);
			break;
		case Stage::Dst:
			controller.SendMessage(Output::ChooseInfluenceDst(phase.m_dsts, phase.m_iSrc >= 0), m_player);
			break;
		case Stage::Discovery:
		{
			Output::ChooseExploreDiscovery msg;
			controller.SendMessage(msg, m_player);
			break;
		}
	}
}

bool InfluenceCmd::IsFinished() const 
{
	return m_stage == Stage::Finished;
}

bool InfluenceCmd::CanUndo() const
{
	return m_stage != Stage::Discovery; 
}

bool InfluenceCmd::Undo()
{
	AssertThrow("InfluenceCmd::Undo", CanUndo()); 
	
	if (m_stage == Stage::Src) 
	{
		if (m_phases.size() == 1)
			return true; 

		m_phases.pop_back();

		const Phase& phase = GetPhase();
		if (phase.m_discovery == DiscoveryType::None)
		{
			m_stage = Stage::Dst;

			TransferDisc(	phase.m_iDst < 0 ? nullptr : &phase.m_dsts[phase.m_iDst], 
							phase.m_iSrc < 0 ? nullptr : &phase.m_srcs[phase.m_iSrc]);

			Controller* pCtrlr = Controller::Get(); // TODO: Something better.

			pCtrlr->SendMessage(Output::UpdateMap(m_game), m_game);
			if (phase.m_iSrc < 0 || phase.m_iDst < 0)
				pCtrlr->SendMessage(Output::UpdateInfluenceTrack(m_team), m_game);
		}
		else
			m_stage = Stage::Discovery;
	}
	else if (m_stage == Stage::Dst) 
		m_stage = Stage::Src;

	return false;
}

void InfluenceCmd::GetSrcChoices()
{
	auto& srcs = GetPhase().m_srcs;

	const Map& map = m_game.GetMap();
	const Map::HexMap& hexes = map.GetHexes();
	for (auto& h : hexes)
		if (h.second->GetOwner() == &m_team)
			srcs.push_back(h.first);
}

void InfluenceCmd::GetDstChoices()
{
	Phase& phase = GetPhase();

	std::set<MapPos> dsts;
	const MapPos* pSrc = phase.m_iSrc < 0 ? nullptr : &phase.m_srcs[phase.m_iSrc];

	const bool bWormholeGen = m_team.HasTech(TechType::WormholeGen);
	const bool bAncientsAlly = Race(m_team.GetRace()).IsAncientsAlly();

	const Map& map = m_game.GetMap();
	const Map::HexMap& hexes = map.GetHexes();
	for (auto& h : hexes)
	{
		if (h.second->GetOwner() == nullptr && h.second->HasShip(&m_team))
			dsts.insert(h.first);
		if (!pSrc || h.first != *pSrc) // Would break the wormhole, see FAQ.
			if (h.second->GetOwner() == &m_team || h.second->HasShip(&m_team))
				map.GetInfluencableNeighbours(h.first, m_team, dsts);
	}

	GetPhase().m_dsts.clear();
	GetPhase().m_dsts.insert(GetPhase().m_dsts.begin(), dsts.begin(), dsts.end());
}

Hex* InfluenceCmd::TransferDisc(const MapPos* pSrcPos, const MapPos* pDstPos)
{
	AssertThrowModel("InfluenceCmd::TransferDisc: no op", pSrcPos != pDstPos);

	if (pSrcPos)
	{
		Hex* pHex = m_game.GetMap().GetHex(*pSrcPos);
		AssertThrowModel("InfluenceCmd::TransferDisc: Invalid src", !!pHex);
		AssertThrowModel("InfluenceCmd::TransferDisc: Src not owned", pHex->GetOwner() == &m_team);
		pHex->SetOwner(nullptr);
	}
	else
		m_team.GetInfluenceTrack().RemoveDiscs(1);

	Hex* pDstHex = nullptr;
	if (pDstPos)
	{
		pDstHex = m_game.GetMap().GetHex(*pDstPos);
		AssertThrowModel("InfluenceCmd::TransferDisc: Invalid dst", !!pDstHex);
		AssertThrowModel("InfluenceCmd::TransferDisc: Dst already owned", pDstHex->GetOwner() == nullptr);
		pDstHex->SetOwner(&m_team);
	}
	else
		m_team.GetInfluenceTrack().AddDiscs(1);
	
	return pDstHex;
}