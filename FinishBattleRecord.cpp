#include "stdafx.h"
#include "FinishBattleRecord.h"
#include "Team.h"
#include "Game.h"
#include "Controller.h"
#include "Output.h"
#include "Battle.h"

FinishBattleRecord::FinishBattleRecord() {}

void FinishBattleRecord::Apply(bool bDo, const RecordContext& context)
{
	GameState& gameState = context.GetGameState();

	context.SendMessage(Output::UpdateShowCombat(context.GetGame(), !bDo));

	if (bDo)
	{
		m_battle = gameState.DetachBattle();
	}
	else
	{
		gameState.AttachBattle(BattlePtr(new Battle(*m_battle)));
	}
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
