#include "stdafx.h"
#include "StartBattleRecord.h"
#include "Team.h"
#include "Game.h"
#include "Controller.h"
#include "Output.h"

StartBattleRecord::StartBattleRecord(const Battle* oldBattle)
{
	if (oldBattle)
	{
		VerifyModel("StartBattleRecord::StartBattleRecord", oldBattle->IsFinished() && !oldBattle->GetGroups().empty());
		m_oldGroups = oldBattle->GetGroups();
	}
}

void StartBattleRecord::Apply(bool bDo, Game& game, const Controller& controller)
{
	controller.SendMessage(Output::UpdateShowCombat(game, bDo), game);

	if (bDo)
	{
		const Hex* hex = game.GetMap().FindPendingBattleHex(game);
		VerifyModel("StartBattleRecord::Apply", !!hex);
		game.AttachBattle(BattlePtr(new Battle(*hex, game, m_oldGroups)));
		controller.SendMessage(Output::UpdateCombat(game, game.GetBattle()), game);
	}
	else
		game.DetachBattle();
}

std::string StartBattleRecord::GetMessage(const Game& game) const
{
	return "battle started";
}

void StartBattleRecord::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveCntr("old_groups", m_oldGroups, Serial::ClassSaver());
}

void StartBattleRecord::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadCntr("old_groups", m_oldGroups, Serial::ClassLoader());
}

REGISTER_DYNAMIC(StartBattleRecord)
