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
#include "CommitSession.h"
#include "UncoloniseCmd.h"
#include "InfluenceRecord.h"

class InfluenceFlipRecord : public TeamRecord
{
public:
	InfluenceFlipRecord() {}
	InfluenceFlipRecord(Colour colour, int flips) : TeamRecord(colour), m_flips(flips) {}

	virtual void Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState) override
	{
		if (bDo)
			teamState.ReturnColonyShips(m_flips);
		else
			teamState.UseColonyShips(m_flips);
	}

	virtual void Update(const Game& game, const Team& team, const RecordContext& context) const override
	{
		context.SendMessage(Output::UpdateColonyShips(team));
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
	if (m_iPhase > 1)
		return std::vector<MapPos>();
	
	return game.GetMap().GetOwnedHexPositions(GetTeam(game));
}

int InfluenceCmd::GetMaxFlips(const LiveGame& game) const
{
	return std::min(m_flipsLeft, GetTeam(game).GetUsedColonyShips());
}

void InfluenceCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	bool canChooseTrack = GetTeam(game).GetInfluenceTrack().GetDiscCount() > 0;
	controller.SendMessage(Output::ChooseInfluenceSrc(GetSources(game), canChooseTrack, true, GetMaxFlips(game)), GetPlayer(game));
}

Cmd::ProcessResult InfluenceCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	if (dynamic_cast<const Input::CmdAbort*>(&msg))
		return nullptr;

	if (dynamic_cast<const Input::CmdInfluenceFlip*>(&msg))
	{
		int flips = GetMaxFlips(session.GetGame());
		VERIFY_INPUT(flips > 0);
		DoRecord(RecordPtr(new InfluenceFlipRecord(m_colour, flips)), session);
		
		return ProcessResult(m_iPhase > 1 ? nullptr : new InfluenceCmd(m_colour, session.GetGame(), m_iPhase, m_flipsLeft - flips));
	}

	auto& m = VerifyCastInput<const Input::CmdInfluenceSrc>(msg);

	std::vector<MapPos> srcs = InfluenceCmd::GetSources(session.GetGame());
	VERIFY_INPUT_MSG("invalid pos index", m.m_iPos == -1 || InRange(srcs, m.m_iPos));

	const LiveGame& game = session.GetGame();
	return ProcessResult(new InfluenceDstCmd(m_colour, game, m.m_iPos < 0 ? nullptr : &srcs[m.m_iPos], m_iPhase, m_flipsLeft));
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

InfluenceDstCmd::InfluenceDstCmd() : m_discovery(DiscoveryType::None), m_flipsLeft(0) {}

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

bool InfluenceDstCmd::CanUndo() const 
{
	return m_discovery == DiscoveryType::None; 
}

int InfluenceDstCmd::GetMaxFlips(const LiveGame& game) const
{
	return std::min(m_flipsLeft, GetTeam(game).GetUsedColonyShips());
}

void InfluenceDstCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseInfluenceDst(GetDests(game), !!m_pSrcPos), GetPlayer(game));
}

Cmd::ProcessResult InfluenceDstCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdInfluenceDst>(msg);

	std::vector<MapPos> dsts = GetDests(session.GetGame());

	VERIFY_INPUT_MSG("invalid pos index", m.m_iPos == -1 || InRange(dsts, m.m_iPos));
	
	const MapPos* pDstPos = m.m_iPos >= 0 ? &dsts[m.m_iPos] : nullptr;
	
	InfluenceRecord* pRec = new InfluenceRecord(m_colour, m_pSrcPos.get(), pDstPos);
	DoRecord(RecordPtr(pRec), session);

	m_discovery = pRec->GetDiscovery();

	const LiveGame& game = session.GetGame();

	const bool bFinish = m_iPhase == 1 && GetMaxFlips(session.GetGame()) == 0;
	Cmd* nextInfluenceCmd = bFinish ? nullptr : new InfluenceCmd(m_colour, game, m_iPhase + 1, m_flipsLeft);
	
	const Map& map = session.GetGame().GetMap();
	
	if (m_discovery != DiscoveryType::None)
	{
		int idHex = map.GetHex(*pDstPos).GetID();
		return ProcessResult(new DiscoverCmd(m_colour, game, m_discovery, idHex), nextInfluenceCmd);
	}
	else if (m_pSrcPos && map.GetHex(*m_pSrcPos).HasPopulation())
		return ProcessResult(new UncoloniseCmd(m_colour, *m_pSrcPos), nextInfluenceCmd);

	return ProcessResult(nextInfluenceCmd);
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
