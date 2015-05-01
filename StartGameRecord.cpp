#include "stdafx.h"
#include "StartGameRecord.h"
#include "Output.h"
#include "Controller.h"
#include "Player.h"
#include "LiveGame.h"

StartGameRecord::StartGameRecord()
{
}

void StartGameRecord::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveArray("hex_piles", m_hexPiles, Serial::ClassSaver());
}

void StartGameRecord::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadArray("hex_piles", m_hexPiles, Serial::ClassLoader());
}
void StartGameRecord::Apply(bool bDo, const Game& game, GameState& gameState) 
{
	gameState.IncrementRound(bDo);

	const int startTech[] = { 12, 12, 14, 16, 18, 20 };
	gameState.ProduceTechnologies(startTech[game.GetTeams().size() - 1], bDo);

	if (m_hexPiles[HexRing::Inner].IsEmpty()) // First time.
		for (auto r : EnumRange<HexRing>())
			m_hexPiles[r] = HexPile::Create(r, (int)game.GetTeams().size());

	for (auto r : EnumRange<HexRing>())
		gameState.GetHexPile(r) = bDo ? m_hexPiles[r] : HexPile();
}

void StartGameRecord::Update(const Game& game, const RecordContext& context) const
{
	context.SendMessage(Output::UpdateRound(game));
	context.SendMessage(Output::UpdateTechnologies(game));
}

std::string StartGameRecord::GetMessage(const Game& game) const
{
	return FormatString("Starting round 1");
}

REGISTER_DYNAMIC(StartGameRecord)

