#include "stdafx.h"
#include "StartBattleRecord.h"
#include "Team.h"
#include "Game.h"
#include "Controller.h"
#include "Output.h"
#include "Battle.h"

StartBattleRecord::StartBattleRecord() {}

void StartBattleRecord::Apply(bool bDo, Game& game, const Controller& controller)
{
	controller.SendMessage(Output::UpdateShowCombat(game, bDo), game);

	if (bDo)
	{
		const Hex* hex = game.GetMap().FindPendingBattleHex(game);
		VerifyModel("StartBattleRecord::Apply", !!hex);
		game.SetBattle(BattlePtr(new Battle(*hex, game)));
		controller.SendMessage(Output::UpdateCombat(game, game.GetBattle()), game);
	}
	else
		game.SetBattle(BattlePtr());
}

std::string StartBattleRecord::GetMessage(const Game& game) const
{
	return "started battle";
}

void StartBattleRecord::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	//node.SaveType("action_name", m_actionName);
}

void StartBattleRecord::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	//node.LoadType("action_name", m_actionName);
}

REGISTER_DYNAMIC(StartBattleRecord)
