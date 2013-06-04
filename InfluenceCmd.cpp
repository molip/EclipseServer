#include "stdafx.h"
#include "InfluenceCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "Player.h"
#include "Race.h"
#include "LiveGame.h"
#include "Map.h"
#include "DiscoverCmd.h"
#include "Record.h"

InfluenceCmd::InfluenceCmd(Player& player, int iPhase) : Cmd(player), m_iPhase(iPhase)
{
	const Map& map = GetGame().GetMap();
	const Map::HexMap& hexes = map.GetHexes();
	for (auto& h : hexes)
		if (h.second->IsOwnedBy(GetTeam()))
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
	AssertThrow("InfluenceCmd::Process: invalid pos index", m.m_iPos == -1 || InRange(m_srcs, m.m_iPos));

	return CmdPtr(new InfluenceDstCmd(m_player, m.m_iPos < 0 ? nullptr : &m_srcs[m.m_iPos], m_iPhase));
}

//-----------------------------------------------------------------------------

class InfluenceRecord : public Record
{
public:
	InfluenceRecord(Colour colour, const MapPos* pSrcPos, const MapPos* pDstPos) : 
		m_colour(colour), m_discovery(DiscoveryType::None) 
	{
		m_pSrcPos.reset(pSrcPos ? new MapPos(*pSrcPos) : nullptr);
		m_pDstPos.reset(pDstPos ? new MapPos(*pDstPos) : nullptr);
	}

	DiscoveryType GetDiscovery() const { return m_discovery; }

	virtual void Do(Game& game, const Controller& controller) override
	{
		Hex* pDstHex = TransferDisc(m_pSrcPos, m_pDstPos, game, controller);

		if (pDstHex && pDstHex->GetDiscoveryTile() != DiscoveryType::None)
		{
			m_discovery = pDstHex->GetDiscoveryTile();
			pDstHex->RemoveDiscoveryTile();
		}
		controller.SendMessage(Output::UpdateMap(game), game);
	}

	virtual void Undo(Game& game, const Controller& controller) override
	{
		if (m_discovery != DiscoveryType::None)
			game.GetMap().GetHex(*m_pDstPos).SetDiscoveryTile(m_discovery);

		TransferDisc(m_pDstPos, m_pSrcPos, game, controller);
		controller.SendMessage(Output::UpdateMap(game), game);
	}

private:
	Hex* TransferDisc(const MapPosPtr& pSrcPos, const MapPosPtr& pDstPos, Game& game, const Controller& controller)
	{
		AssertThrowModel("InfluenceRecord::TransferDisc: no op", pSrcPos != pDstPos && !(pSrcPos && pDstPos && *pSrcPos == *pDstPos));
		Team& team = game.GetTeam(m_colour);

		if (pSrcPos)
		{
			Hex& hex = game.GetMap().GetHex(*pSrcPos);
			AssertThrowModel("InfluenceCmd::TransferDisc: Src not owned", hex.IsOwnedBy(team));
			hex.SetColour(Colour::None);
		}
		else
			team.GetInfluenceTrack().RemoveDiscs(1);

		Hex* pDstHex = nullptr;
		if (pDstPos)
		{
			pDstHex = &game.GetMap().GetHex(*pDstPos);
			AssertThrowModel("InfluenceCmd::TransferDisc: Dst already owned", !pDstHex->IsOwned());
			pDstHex->SetColour(m_colour);
		}
		else
			team.GetInfluenceTrack().AddDiscs(1);
	
		if (!pSrcPos || !pDstPos)
			controller.SendMessage(Output::UpdateInfluenceTrack(team), game);

		return pDstHex;
	}

	Colour m_colour;
	MapPosPtr m_pSrcPos, m_pDstPos;
	DiscoveryType m_discovery;
};

//-----------------------------------------------------------------------------

InfluenceDstCmd::InfluenceDstCmd(Player& player, const MapPos* pSrcPos, int iPhase) : Cmd(player), m_iPhase(iPhase),
	m_pDstPos(nullptr), m_discovery(DiscoveryType::None)
{
	if (pSrcPos)
		m_pSrcPos.reset(new MapPos(*pSrcPos));

	std::set<MapPos> dsts;

	const bool bWormholeGen = GetTeam().HasTech(TechType::WormholeGen);

	const Map& map = GetGame().GetMap();
	const Map::HexMap& hexes = map.GetHexes();
	for (auto& h : hexes)
	{
		if (!h.second->IsOwned())
			if (h.second->HasShip(&GetTeam()) && !h.second->HasForeignShip(GetGame(), &GetTeam())) // "a hex where only you have a Ship"
				dsts.insert(h.first);
		if (!pSrcPos || h.first != *pSrcPos) // Would break the wormhole, see FAQ.
			if (h.second->IsOwnedBy(GetTeam()) || h.second->HasShip(&GetTeam())) // "adjacent to a hex where you have a disc or a Ship"
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
	AssertThrow("InfluenceDstCmd::Process: invalid pos index", m.m_iPos == -1 || InRange(m_dsts, m.m_iPos));
	
	const MapPos* pDstPos = m.m_iPos >= 0 ? &m_dsts[m.m_iPos] : nullptr;
	
	InfluenceRecord* pRec = new InfluenceRecord(GetTeam().GetColour(), m_pSrcPos.get(), pDstPos);
	pRec->Do(GetGame(), controller);
	m_discovery = pRec->GetDiscovery();
	GetGame().PushRecord(RecordPtr(pRec));

	Cmd* pNext = m_iPhase == 0 ? new InfluenceCmd(m_player, 1) : nullptr;

	if (m_discovery != DiscoveryType::None)
		pNext = new DiscoverCmd(m_player, m_discovery, CmdPtr(pNext));

	return CmdPtr(pNext);
}

void InfluenceDstCmd::Undo(const Controller& controller)
{
	RecordPtr pRec = GetGame().PopRecord();
	pRec->Undo(GetGame(), controller);
}
