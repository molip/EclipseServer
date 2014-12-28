#include "stdafx.h"
#include "AdvanceCombatTurnRecord.h"
#include "Team.h"
#include "Game.h"
#include "Controller.h"
#include "Output.h"

AdvanceCombatTurnRecord::AdvanceCombatTurnRecord()
{
}

void AdvanceCombatTurnRecord::Apply(bool bDo, const RecordContext& context)
{
	Game& game = context.GetGame();
	GameState& gameState = context.GetGameState();

	Battle& battle = gameState.GetBattle();
	if (bDo)
		m_oldTurn = battle.AdvanceTurn(game);
	else
		battle.SetTurn(m_oldTurn);

	if (!battle.IsFinished())
		context.SendMessage(Output::UpdateCombat(game, battle));
}

std::string AdvanceCombatTurnRecord::GetMessage(const Game& game) const
{
	return "advance combat turn";
}

void AdvanceCombatTurnRecord::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveClass("old_turn", m_oldTurn);
}

void AdvanceCombatTurnRecord::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadClass("old_turn", m_oldTurn);
}

REGISTER_DYNAMIC(AdvanceCombatTurnRecord)
