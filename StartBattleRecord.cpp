#include "stdafx.h"
#include "StartBattleRecord.h"
#include "Team.h"
#include "Game.h"
#include "Controller.h"
#include "Output.h"
#include "ShipBattle.h"
#include "PopulationBattle.h"

StartBattleRecord::StartBattleRecord(const Battle* oldBattle, int hexId) : m_hexId(hexId)
{
	if (oldBattle)
	{
		VERIFY_MODEL(oldBattle->IsFinished() && !oldBattle->GetGroups().empty());
		m_oldGroups = oldBattle->GetGroups();
	}
}

void StartBattleRecord::Apply(bool bDo, const Game& game, GameState& gameState)
{
	if (bDo)
	{
		const Hex* hex = game.GetMap().FindHex(m_hexId);
		VERIFY_MODEL(!!hex);
		if (hex->CanAttackPopulation())
			gameState.AttachBattle(BattlePtr(new PopulationBattle(*hex, game, m_oldGroups)));
		else
			gameState.AttachBattle(BattlePtr(new ShipBattle(*hex, game, m_oldGroups)));
	}
	else
		gameState.DetachBattle();
}

void StartBattleRecord::Update(const Game& game, const RecordContext& context) const
{
	context.SendMessage(Output::UpdateShowCombat(game, game.HasBattle()));
	
	if (game.HasBattle())
		context.SendMessage(Output::UpdateCombat(game, game.GetBattle()));
}

std::string StartBattleRecord::GetMessage(const Game& game) const
{
	return "battle started";
}

void StartBattleRecord::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveCntr("old_groups", m_oldGroups, Serial::ClassSaver());
	node.SaveType("hex_id", m_hexId);
}

void StartBattleRecord::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadCntr("old_groups", m_oldGroups, Serial::ClassLoader());
	node.LoadType("hex_id", m_hexId);
}

REGISTER_DYNAMIC(StartBattleRecord)
