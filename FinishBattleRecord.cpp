#include "stdafx.h"
#include "FinishBattleRecord.h"
#include "Team.h"
#include "Game.h"
#include "Controller.h"
#include "Output.h"
#include "Battle.h"

FinishBattleRecord::FinishBattleRecord() {}

void FinishBattleRecord::Apply(bool bDo, Game& game, const Controller& controller)
{
	controller.SendMessage(Output::UpdateShowCombat(game, !bDo), game);

	if (bDo)
	{
		m_battle = game.DetachBattle();
	}
	else
	{
		game.AttachBattle(std::move(m_battle));
		controller.SendMessage(Output::UpdateCombat(game, game.GetBattle()), game);
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