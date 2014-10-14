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
#include "CommitSession.h"

class InfluenceFlipRecord : public TeamRecord
{
public:
	InfluenceFlipRecord() {}
	InfluenceFlipRecord(Colour colour, int flips) : TeamRecord(colour), m_flips(flips) {}

	virtual void Apply(bool bDo, Game& game, const Controller& controller) override
	{
		Team& team = game.GetTeam(m_colour);

		if (bDo)
			team.ReturnColonyShips(m_flips);
		else
			team.UseColonyShips(m_flips);

		controller.SendMessage(Output::UpdateColonyShips(team), game);
	}

	virtual std::string GetTeamMessage() const
	{
		return ::FormatString("flipped %0 colony ships", m_flips);
	}

	virtual void Save(Serial::SaveNode& node) const override
	{
		__super::Save(node);
		node.SaveType("flips", m_flips);
	}

	virtual void Load(const Serial::LoadNode& node) override
	{
		__super::Load(node);
		node.LoadType("flips", m_flips);
	}

private:
	int m_flips;
};

REGISTER_DYNAMIC(InfluenceFlipRecord)

//-----------------------------------------------------------------------------

InfluenceCmd::InfluenceCmd(Colour colour, const LiveGame& game, int iPhase, int flipsLeft) : PhaseCmd(colour, iPhase), m_flipsLeft(flipsLeft)
{
	VERIFY_MODEL(!GetTeam(game).HasPassed());
}

std::vector<MapPos> InfluenceCmd::GetSources(const LiveGame& game) const
{
	std::vector<MapPos> srcs;

	if (m_iPhase <= 1)
	{
		const Team& team = GetTeam(game);
		const Map& map = game.GetMap();
		const Map::HexMap& hexes = map.GetHexes();
		for (auto& h : hexes)
			if (h.second->IsOwnedBy(team))
				srcs.push_back(h.first);
	}
	return srcs;
}

int InfluenceCmd::GetMaxFlips(const LiveGame& game) const
{
	return std::min(m_flipsLeft, GetTeam(game).GetUsedColonyShips());
}

void InfluenceCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	bool canChooseTrack = GetTeam(game).GetInfluenceTrack().GetDiscCount() > 0;
	controller.SendMessage(Output::ChooseInfluenceSrc(GetSources(game), canChooseTrack, GetMaxFlips(game)), GetPlayer(game));
}

CmdPtr InfluenceCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	if (dynamic_cast<const Input::CmdAbort*>(&msg))
		return nullptr;

	if (dynamic_cast<const Input::CmdInfluenceFlip*>(&msg))
	{
		int flips = GetMaxFlips(session.GetGame());
		VERIFY_INPUT(flips > 0);
		DoRecord(RecordPtr(new InfluenceFlipRecord(m_colour, flips)), session);
		
		return m_iPhase > 1 ? nullptr : CmdPtr(new InfluenceCmd(m_colour, session.GetGame(), m_iPhase, m_flipsLeft - flips));
	}

	auto& m = VerifyCastInput<const Input::CmdInfluenceSrc>(msg);

	std::vector<MapPos> srcs = InfluenceCmd::GetSources(session.GetGame());
	VERIFY_INPUT_MSG("invalid pos index", m.m_iPos == -1 || InRange(srcs, m.m_iPos));

	const LiveGame& game = session.GetGame();
	return CmdPtr(new InfluenceDstCmd(m_colour, game, m.m_iPos < 0 ? nullptr : &srcs[m.m_iPos], m_iPhase, m_flipsLeft));
}

void InfluenceCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveType("flips_left", m_flipsLeft);
}

void InfluenceCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadType("flips_left", m_flipsLeft);
}

REGISTER_DYNAMIC(InfluenceCmd)

//-----------------------------------------------------------------------------

class InfluenceRecord : public TeamRecord
{
public:
	InfluenceRecord() : m_discovery(DiscoveryType::None), m_srcID(0), m_dstID(0){}

	InfluenceRecord(Colour colour, const MapPos* pSrcPos, const MapPos* pDstPos) : 
		TeamRecord(colour), m_discovery(DiscoveryType::None) 
	{
		m_pSrcPos.reset(pSrcPos ? new MapPos(*pSrcPos) : nullptr);
		m_pDstPos.reset(pDstPos ? new MapPos(*pDstPos) : nullptr);
	}

	DiscoveryType GetDiscovery() const { return m_discovery; }

	virtual void Apply(bool bDo, Game& game, const Controller& controller) override
	{
		if (bDo)
		{
			m_srcID = m_pSrcPos ? game.GetMap().GetHex(*m_pSrcPos).GetID() : 0;
			m_dstID = m_pDstPos ? game.GetMap().GetHex(*m_pDstPos).GetID() : 0;
			
			Hex* pDstHex = TransferDisc(m_pSrcPos, m_pDstPos, game, controller);

			if (pDstHex && pDstHex->GetDiscoveryTile() != DiscoveryType::None)
			{
				m_discovery = pDstHex->GetDiscoveryTile();
				pDstHex->RemoveDiscoveryTile();
			}
		}
		else
		{
			if (m_discovery != DiscoveryType::None)
				game.GetMap().GetHex(*m_pDstPos).SetDiscoveryTile(m_discovery);

			TransferDisc(m_pDstPos, m_pSrcPos, game, controller);
		}
		controller.SendMessage(Output::UpdateMap(game), game);
	}

	virtual void Save(Serial::SaveNode& node) const override 
	{
		__super::Save(node);
		node.SaveEnum("discovery", m_discovery);
		node.SaveTypePtr("src_pos", m_pSrcPos);
		node.SaveTypePtr("dst_pos", m_pDstPos);
		node.SaveType("src_id", m_srcID);
		node.SaveType("dst_id", m_dstID);
	}
	
	virtual void Load(const Serial::LoadNode& node) override 
	{
		__super::Load(node);
		node.LoadEnum("discovery", m_discovery);
		node.LoadTypePtr("src_pos", m_pSrcPos);
		node.LoadTypePtr("dst_pos", m_pDstPos);
		node.LoadType("src_id", m_srcID);
		node.LoadType("dst_id", m_dstID);
	}

private:
	Hex* TransferDisc(const MapPosPtr& pSrcPos, const MapPosPtr& pDstPos, Game& game, const Controller& controller)
	{
		VERIFY_MODEL_MSG("no op", pSrcPos != pDstPos && !(pSrcPos && pDstPos && *pSrcPos == *pDstPos));
		Team& team = game.GetTeam(m_colour);

		if (pSrcPos)
		{
			Hex& hex = game.GetMap().GetHex(*pSrcPos);
			VERIFY_MODEL_MSG("Src not owned", hex.IsOwnedBy(team));
			hex.SetColour(Colour::None);
		}
		else
			team.GetInfluenceTrack().RemoveDiscs(1);

		Hex* pDstHex = nullptr;
		if (pDstPos)
		{
			pDstHex = &game.GetMap().GetHex(*pDstPos);
			VERIFY_MODEL_MSG("Dst already owned", !pDstHex->IsOwned());
			pDstHex->SetColour(m_colour);
		}
		else
			team.GetInfluenceTrack().AddDiscs(1);
	
		if (!pSrcPos || !pDstPos)
			controller.SendMessage(Output::UpdateInfluenceTrack(team), game);

		return pDstHex;
	}

	virtual std::string GetTeamMessage() const
	{
		auto getDesc = [&](int id)
		{
			return id ? FormatString("hex %0", id) : "influence track";
		};

		return FormatString("moved an influence disc from %0 to %1", getDesc(m_srcID), getDesc(m_dstID));
	}

	int m_srcID, m_dstID;
	MapPosPtr m_pSrcPos, m_pDstPos;
	DiscoveryType m_discovery;
};

REGISTER_DYNAMIC(InfluenceRecord)

//-----------------------------------------------------------------------------

class DiscoverAndInfluenceCmd : public DiscoverCmd
{
public:
	DiscoverAndInfluenceCmd() {}
	DiscoverAndInfluenceCmd(Colour colour, const LiveGame& game, DiscoveryType discovery, int flipsLeft) :
		DiscoverCmd(colour, game, discovery), m_flipsLeft(flipsLeft) {}
private:
	virtual CmdPtr GetNextCmd(const LiveGame& game) const override { return CmdPtr(new InfluenceCmd(m_colour, game, 1, m_flipsLeft)); }

	virtual void Save(Serial::SaveNode& node) const override
	{
		__super::Save(node);
		node.SaveType("flips_left", m_flipsLeft);
	}

	virtual void Load(const Serial::LoadNode& node) override
	{
		__super::Load(node);
		node.LoadType("flips_left", m_flipsLeft);
	}

	int m_flipsLeft;
};

REGISTER_DYNAMIC(DiscoverAndInfluenceCmd)

//-----------------------------------------------------------------------------

InfluenceDstCmd::InfluenceDstCmd(Colour colour, const LiveGame& game, const MapPos* pSrcPos, int iPhase, int flipsLeft) : 
PhaseCmd(colour, iPhase), m_discovery(DiscoveryType::None), m_flipsLeft(flipsLeft)
{
	if (pSrcPos)
		m_pSrcPos.reset(new MapPos(*pSrcPos));
}

std::vector<MapPos> InfluenceDstCmd::GetDests(const LiveGame& game) const
{
	auto& team = GetTeam(game);

	std::set<MapPos> dsts;

	const bool bWormholeGen = team.HasTech(TechType::WormholeGen);

	const Map& map = game.GetMap();
	for (auto& h : map.GetHexes())
	{
		if (!h.second->IsOwned())
			if (h.second->HasShip(team.GetColour()) && !h.second->HasForeignShip(team.GetColour())) // "a hex where only you have a Ship"
				dsts.insert(h.first);
		if (!m_pSrcPos || h.first != *m_pSrcPos) // Would break the wormhole, see FAQ.
			if (h.second->IsOwnedBy(team) || h.second->HasShip(team.GetColour())) // "adjacent to a hex where you have a disc or a Ship"
				map.GetInfluencableNeighbours(h.first, team, dsts);
	}

	return std::vector<MapPos>(dsts.begin(), dsts.end());
}

void InfluenceDstCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseInfluenceDst(GetDests(game), !!m_pSrcPos), GetPlayer(game));
}

CmdPtr InfluenceDstCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdInfluenceDst>(msg);

	std::vector<MapPos> dsts = GetDests(session.GetGame());

	VERIFY_INPUT_MSG("invalid pos index", m.m_iPos == -1 || InRange(dsts, m.m_iPos));
	
	const MapPos* pDstPos = m.m_iPos >= 0 ? &dsts[m.m_iPos] : nullptr;
	
	InfluenceRecord* pRec = new InfluenceRecord(m_colour, m_pSrcPos.get(), pDstPos);
	DoRecord(RecordPtr(pRec), session);

	m_discovery = pRec->GetDiscovery();

	const bool bFinish = m_iPhase == 1 && m_flipsLeft == 0;

	const LiveGame& game = session.GetGame();
	if (m_discovery != DiscoveryType::None)
		return CmdPtr(bFinish ? new DiscoverCmd(m_colour, game, m_discovery) : new DiscoverAndInfluenceCmd(m_colour, game, m_discovery, m_flipsLeft));

	return CmdPtr(bFinish ? nullptr : new InfluenceCmd(m_colour, game, m_iPhase + 1, m_flipsLeft));
}

void InfluenceDstCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveTypePtr("src", m_pSrcPos);
	node.SaveEnum("discovery", m_discovery);
	node.SaveType("flips_left", m_flipsLeft);
}

void InfluenceDstCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadTypePtr("src", m_pSrcPos);
	node.LoadEnum("discovery", m_discovery);
	node.LoadType("flips_left", m_flipsLeft);
}

REGISTER_DYNAMIC(InfluenceDstCmd)
