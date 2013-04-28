#include "stdafx.h"
#include "InfluenceCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "Player.h"
#include "Race.h"
#include "Game.h"
#include "Map.h"
#include "DiscoverCmd.h"

InfluenceCmd::InfluenceCmd(Player& player, int iPhase) : Cmd(player), m_iPhase(iPhase)
{
	const Map& map = GetGame().GetMap();
	const Map::HexMap& hexes = map.GetHexes();
	for (auto& h : hexes)
		if (h.second->GetOwner() == &GetTeam())
			m_srcs.push_back(h.first);
}

void InfluenceCmd::UpdateClient(const Controller& controller) const
{
	controller.SendMessage(Output::ChooseInfluenceSrc(m_srcs, GetTeam().GetInfluenceTrack().GetDiscCount() > 0), m_player);
}

CmdPtr InfluenceCmd::Process(const Input::CmdMessage& msg, const Controller& controller)
{
	// TODO: Flip colony ships.

	if (dynamic_cast<const Input::CmdExploreReject*>(&msg))
		return nullptr;

	auto& m = CastThrow<const Input::CmdInfluenceSrc>(msg);
	AssertThrow("InfluenceCmd::Process (Stage::Src): invalid pos index", m.m_iPos == -1 || InRange(m_srcs, m.m_iPos));

	return CmdPtr(new InfluenceDstCmd(m_player, m.m_iPos < 0 ? nullptr : &m_srcs[m.m_iPos], m_iPhase));
}

//-----------------------------------------------------------------------------

InfluenceDstCmd::InfluenceDstCmd(Player& player, const MapPos* pSrcPos, int iPhase) : Cmd(player), m_iPhase(iPhase),
	m_pDstPos(nullptr), m_discovery(DiscoveryType::None)
{
	if (pSrcPos)
		m_pSrcPos.reset(new MapPos(*pSrcPos));

	std::set<MapPos> dsts;

	const bool bWormholeGen = GetTeam().HasTech(TechType::WormholeGen);
	const bool bAncientsAlly = Race(GetTeam().GetRace()).IsAncientsAlly();

	const Map& map = GetGame().GetMap();
	const Map::HexMap& hexes = map.GetHexes();
	for (auto& h : hexes)
	{
		if (h.second->GetOwner() == nullptr && h.second->HasShip(&GetTeam()))
			dsts.insert(h.first);
		if (!pSrcPos || h.first != *pSrcPos) // Would break the wormhole, see FAQ.
			if (h.second->GetOwner() == &GetTeam() || h.second->HasShip(&GetTeam()))
				map.GetInfluencableNeighbours(h.first, GetTeam(), dsts);
	}

	m_dsts.insert(m_dsts.begin(), dsts.begin(), dsts.end());
}

void InfluenceDstCmd::UpdateClient(const Controller& controller) const
{
	controller.SendMessage(Output::ChooseInfluenceDst(m_dsts, !!m_pSrcPos), m_player);
}

CmdPtr InfluenceDstCmd::Process(const Input::CmdMessage& msg, const Controller& controller)
{
	auto& m = CastThrow<const Input::CmdInfluenceDst>(msg);
	AssertThrow("InfluenceCmd::Process (Stage::Dst): invalid pos index", m.m_iPos == -1 || InRange(m_dsts, m.m_iPos));
	
	m_pDstPos = m.m_iPos >= 0 ? &m_dsts[m.m_iPos] : nullptr;
	
	Hex* pDstHex = TransferDisc(m_pSrcPos.get(), m_pDstPos, controller);

	Cmd* pNext = m_iPhase == 0 ? new InfluenceCmd(m_player, 1) : nullptr;

	if (pDstHex && pDstHex->GetDiscoveryTile() != DiscoveryType::None)
	{
		pNext = new DiscoverCmd(m_player, pDstHex->GetDiscoveryTile(), CmdPtr(pNext));
		pDstHex->RemoveDiscoveryTile();
	}

	controller.SendMessage(Output::UpdateMap(GetGame()), GetGame());

	return CmdPtr(pNext);
}

void InfluenceDstCmd::Undo(const Controller& controller)
{
	TransferDisc(m_pDstPos, m_pSrcPos.get(), controller);
	controller.SendMessage(Output::UpdateMap(GetGame()), GetGame());
}

Hex* InfluenceDstCmd::TransferDisc(const MapPos* pSrcPos, const MapPos* pDstPos, const Controller& controller)
{
	AssertThrowModel("InfluenceCmd::TransferDisc: no op", pSrcPos != pDstPos);

	if (pSrcPos)
	{
		Hex* pHex = GetGame().GetMap().GetHex(*pSrcPos);
		AssertThrowModel("InfluenceCmd::TransferDisc: Invalid src", !!pHex);
		AssertThrowModel("InfluenceCmd::TransferDisc: Src not owned", pHex->GetOwner() == &GetTeam());
		pHex->SetOwner(nullptr);
	}
	else
		GetTeam().GetInfluenceTrack().RemoveDiscs(1);

	Hex* pDstHex = nullptr;
	if (pDstPos)
	{
		pDstHex = GetGame().GetMap().GetHex(*pDstPos);
		AssertThrowModel("InfluenceCmd::TransferDisc: Invalid dst", !!pDstHex);
		AssertThrowModel("InfluenceCmd::TransferDisc: Dst already owned", pDstHex->GetOwner() == nullptr);
		pDstHex->SetOwner(&GetTeam());
	}
	else
		GetTeam().GetInfluenceTrack().AddDiscs(1);
	
	if (!pSrcPos || !pDstPos)
		controller.SendMessage(Output::UpdateInfluenceTrack(GetTeam()), GetGame());

	return pDstHex;
}
