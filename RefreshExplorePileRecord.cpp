#include "stdafx.h"
#include "RefreshExplorePileRecord.h"
#include "Output.h"
#include "Controller.h"
#include "Player.h"
#include "LiveGame.h"

RefreshExplorePileRecord::RefreshExplorePileRecord() : m_ring(HexRing::None)
{
}

RefreshExplorePileRecord::RefreshExplorePileRecord(HexRing ring) : m_ring(ring)
{
}

void RefreshExplorePileRecord::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveEnum("ring", m_ring);
	node.SaveCntr("discard_pile", m_discardPile, Serial::TypeSaver());
	node.SaveCntr("new_pile", m_newPile, Serial::TypeSaver());
}
	
void RefreshExplorePileRecord::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadEnum("ring", m_ring);
	node.LoadCntr("discard_pile", m_discardPile, Serial::TypeLoader());
	node.LoadCntr("new_pile", m_newPile, Serial::TypeLoader());
}

void RefreshExplorePileRecord::Apply(bool bDo, const Game& game, GameState& gameState) 
{
	auto& pile = gameState.GetHexPile(m_ring).GetTiles();
	auto& discard = gameState.GetHexDiscardPile(m_ring).GetTiles();

	if (bDo)
	{
		VERIFY_MODEL(!discard.empty() && pile.empty());

		if (m_newPile.empty()) // First time. 
		{
			m_newPile = m_discardPile = discard;
			std::shuffle(m_newPile.begin(), m_newPile.end(), GetRandom());
		}

		VERIFY_MODEL(discard == m_discardPile);
		discard.clear();
		pile = m_newPile;
	}
	else
	{
		VERIFY_MODEL(pile == m_newPile);
		pile.clear();
		discard = m_discardPile;
	}
}

void RefreshExplorePileRecord::Update(const Game& game, const RecordContext& context) const
{
}

std::string RefreshExplorePileRecord::GetMessage(const Game& game) const
{
	return "Refreshed hex pile: "s + ::EnumToString(m_ring);
}

REGISTER_DYNAMIC(RefreshExplorePileRecord)
