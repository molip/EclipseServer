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

InfluenceCmd::InfluenceCmd(Colour colour, LiveGame& game, int iPhase) : PhaseCmd(colour, iPhase)
{
	Team& team = GetTeam(game);
	AssertThrow("InfluenceCmd::InfluenceCmd", !team.HasPassed());

	const Map& map = game.GetMap();
	const Map::HexMap& hexes = map.GetHexes();
	for (auto& h : hexes)
		if (h.second->IsOwnedBy(team))
			m_srcs.push_back(h.first);
}

void InfluenceCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseInfluenceSrc(m_srcs, GetTeam(game).GetInfluenceTrack().GetDiscCount() > 0), GetPlayer(game));
}

CmdPtr InfluenceCmd::Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game)
{
	// TODO: Flip colony ships.

	if (dynamic_cast<const Input::CmdAbort*>(&msg))
		return nullptr;

	auto& m = CastThrow<const Input::CmdInfluenceSrc>(msg);
	AssertThrow("InfluenceCmd::Process: invalid pos index", m.m_iPos == -1 || InRange(m_srcs, m.m_iPos));

	return CmdPtr(new InfluenceDstCmd(m_colour, game, m.m_iPos < 0 ? nullptr : &m_srcs[m.m_iPos], m_iPhase));
}

void InfluenceCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveCntr("srcs", m_srcs, Serial::TypeSaver());
}

void InfluenceCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadCntr("srcs", m_srcs, Serial::TypeLoader());
}

REGISTER_DYNAMIC(InfluenceCmd)

//-----------------------------------------------------------------------------

class InfluenceRecord : public Record
{
public:
	InfluenceRecord() : m_discovery(DiscoveryType::None) {}

	InfluenceRecord(Colour colour, const MapPos* pSrcPos, const MapPos* pDstPos) : 
		Record(colour), m_discovery(DiscoveryType::None) 
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

	virtual void Save(Serial::SaveNode& node) const override 
	{
		__super::Save(node);
		node.SaveEnum("discovery", m_discovery);
		node.SaveTypePtr("src_pos", m_pSrcPos);
		node.SaveTypePtr("dst_pos", m_pDstPos);
	}
	
	virtual void Load(const Serial::LoadNode& node) override 
	{
		__super::Load(node);
		node.LoadEnum("discovery", m_discovery);
		node.LoadTypePtr("src_pos", m_pSrcPos);
		node.LoadTypePtr("dst_pos", m_pDstPos);
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

	MapPosPtr m_pSrcPos, m_pDstPos;
	DiscoveryType m_discovery;
};

REGISTER_DYNAMIC(InfluenceRecord)

//-----------------------------------------------------------------------------

class DiscoverAndInfluenceCmd : public DiscoverCmd
{
public:
	DiscoverAndInfluenceCmd() {}
	DiscoverAndInfluenceCmd(Colour colour, LiveGame& game, DiscoveryType discovery) : DiscoverCmd(colour, game, discovery){}
private:
	virtual CmdPtr GetNextCmd(LiveGame& game) const override { return CmdPtr(new InfluenceCmd(m_colour, game, 1)); }
};

REGISTER_DYNAMIC(DiscoverAndInfluenceCmd)

//-----------------------------------------------------------------------------

InfluenceDstCmd::InfluenceDstCmd(Colour colour, LiveGame& game, const MapPos* pSrcPos, int iPhase) : PhaseCmd(colour, iPhase),
	m_discovery(DiscoveryType::None)
{
	auto& team = GetTeam(game);

	if (pSrcPos)
		m_pSrcPos.reset(new MapPos(*pSrcPos));

	std::set<MapPos> dsts;

	const bool bWormholeGen = team.HasTech(TechType::WormholeGen);

	const Map& map = game.GetMap();
	const Map::HexMap& hexes = map.GetHexes();
	for (auto& h : hexes)
	{
		if (!h.second->IsOwned())
			if (h.second->HasShip(&team) && !h.second->HasForeignShip(game, &team)) // "a hex where only you have a Ship"
				dsts.insert(h.first);
		if (!pSrcPos || h.first != *pSrcPos) // Would break the wormhole, see FAQ.
			if (h.second->IsOwnedBy(team) || h.second->HasShip(&team)) // "adjacent to a hex where you have a disc or a Ship"
				map.GetInfluencableNeighbours(h.first, team, dsts);
	}

	m_dsts.insert(m_dsts.begin(), dsts.begin(), dsts.end());
}

void InfluenceDstCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseInfluenceDst(m_dsts, !!m_pSrcPos), GetPlayer(game));
}

CmdPtr InfluenceDstCmd::Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game)
{
	auto& m = CastThrow<const Input::CmdInfluenceDst>(msg);
	AssertThrow("InfluenceDstCmd::Process: invalid pos index", m.m_iPos == -1 || InRange(m_dsts, m.m_iPos));
	
	const MapPos* pDstPos = m.m_iPos >= 0 ? &m_dsts[m.m_iPos] : nullptr;
	
	InfluenceRecord* pRec = new InfluenceRecord(m_colour, m_pSrcPos.get(), pDstPos);
	DoRecord(RecordPtr(pRec), controller, game);

	m_discovery = pRec->GetDiscovery();

	const bool bFinish = m_iPhase == 1;

	if (m_discovery != DiscoveryType::None)
		return CmdPtr(bFinish ? new DiscoverCmd(m_colour, game, m_discovery) : new DiscoverAndInfluenceCmd(m_colour, game, m_discovery));

	return CmdPtr(bFinish ? nullptr : new InfluenceCmd(m_colour, game, 1));
}

void InfluenceDstCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveTypePtr("src", m_pSrcPos);
	node.SaveCntr("dsts", m_dsts, Serial::TypeSaver());
	node.SaveEnum("discovery", m_discovery);
}

void InfluenceDstCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadTypePtr("src", m_pSrcPos);
	node.LoadCntr("dsts", m_dsts, Serial::TypeLoader());
	node.LoadEnum("discovery", m_discovery);
}

REGISTER_DYNAMIC(InfluenceDstCmd)
