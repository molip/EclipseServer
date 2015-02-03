#include "stdafx.h"
#include "InfluenceRecord.h"
#include "Output.h"
#include "Game.h"
#include "Map.h"

InfluenceRecord::InfluenceRecord() : m_discovery(DiscoveryType::None), m_srcID(0), m_dstID(0){}

InfluenceRecord::InfluenceRecord(Colour colour, const MapPos* pSrcPos, const MapPos* pDstPos) :
	TeamRecord(colour), m_discovery(DiscoveryType::None) 
{
	m_pSrcPos.reset(pSrcPos ? new MapPos(*pSrcPos) : nullptr);
	m_pDstPos.reset(pDstPos ? new MapPos(*pDstPos) : nullptr);
}

void InfluenceRecord::Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState)
{
	if (bDo)
	{
		m_srcID = m_pSrcPos ? game.GetMap().GetHex(*m_pSrcPos).GetID() : 0;
		m_dstID = m_pDstPos ? game.GetMap().GetHex(*m_pDstPos).GetID() : 0;
			
		Hex* pDstHex = TransferDisc(m_pSrcPos, m_pDstPos, gameState, team, teamState);

		if (pDstHex && pDstHex->GetDiscoveryTile() != DiscoveryType::None)
		{
			m_discovery = pDstHex->GetDiscoveryTile();
			pDstHex->RemoveDiscoveryTile();
		}
	}
	else
	{
		if (m_discovery != DiscoveryType::None)
			gameState.GetMap().GetHex(*m_pDstPos).SetDiscoveryTile(m_discovery);

		TransferDisc(m_pDstPos, m_pSrcPos, gameState, team, teamState);
	}
}

void InfluenceRecord::Update(const Game& game, const Team& team, const RecordContext& context) const 
{
	context.SendMessage(Output::UpdateMap(game));
	context.SendMessage(Output::UpdateInfluenceTrack(team));
}

void InfluenceRecord::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveEnum("discovery", m_discovery);
	node.SaveTypePtr("src_pos", m_pSrcPos);
	node.SaveTypePtr("dst_pos", m_pDstPos);
	node.SaveType("src_id", m_srcID);
	node.SaveType("dst_id", m_dstID);
}
	
void InfluenceRecord::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadEnum("discovery", m_discovery);
	node.LoadTypePtr("src_pos", m_pSrcPos);
	node.LoadTypePtr("dst_pos", m_pDstPos);
	node.LoadType("src_id", m_srcID);
	node.LoadType("dst_id", m_dstID);
}

Hex* InfluenceRecord::TransferDisc(const MapPosPtr& pSrcPos, const MapPosPtr& pDstPos, GameState& gameState, const Team& team, TeamState& teamState)
{
	VERIFY_MODEL_MSG("no op", pSrcPos != pDstPos && !(pSrcPos && pDstPos && *pSrcPos == *pDstPos));

	if (pSrcPos)
	{
		Hex& hex = gameState.GetMap().GetHex(*pSrcPos);
		VERIFY_MODEL_MSG("Src not owned", hex.IsOwnedBy(team));
		hex.SetColour(Colour::None);
	}
	else
		teamState.GetInfluenceTrack().RemoveDiscs(1);

	Hex* pDstHex = nullptr;
	if (pDstPos)
	{
		pDstHex = &gameState.GetMap().GetHex(*pDstPos);
		VERIFY_MODEL_MSG("Dst already owned", !pDstHex->IsOwned());
		pDstHex->SetColour(m_colour);
	}
	else
		teamState.GetInfluenceTrack().AddDiscs(1);
	
	return pDstHex;
}

std::string InfluenceRecord::GetTeamMessage() const
{
	auto getDesc = [&](int id)
	{
		return id ? FormatString("hex %0", id) : "influence track";
	};

	return FormatString("moved an influence disc from %0 to %1", getDesc(m_srcID), getDesc(m_dstID));
}

REGISTER_DYNAMIC(InfluenceRecord)
