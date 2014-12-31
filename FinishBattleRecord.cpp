#include "stdafx.h"
#include "FinishBattleRecord.h"
#include "Team.h"
#include "Game.h"
#include "Controller.h"
#include "Output.h"
#include "Battle.h"

FinishBattleRecord::FinishBattleRecord() {}

void FinishBattleRecord::Apply(bool bDo, const Game& game, GameState& gameState)
{
	if (bDo)
	{
		m_battle = gameState.DetachBattle();
	}
	else
	{
		gameState.AttachBattle(BattlePtr(new Battle(*m_battle)));
	}
}

void FinishBattleRecord::Update(const Game& game, const RecordContext& context) const
{
	context.SendMessage(Output::UpdateShowCombat(game, game.HasBattle()));
}

std::string FinishBattleRecord::GetMessage(const Game& game) const
{
	return "battle finished";
}

void FinishBattleRecord::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveClassPtr("battle", m_battle);
}

void FinishBattleRecord::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadClassPtr("battle", m_battle);
}

REGISTER_DYNAMIC(FinishBattleRecord)
