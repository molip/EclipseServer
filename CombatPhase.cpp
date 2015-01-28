#include "stdafx.h"
#include "CombatPhase.h"
#include "CmdStack.h"
#include "LiveGame.h"
#include "Controller.h"
#include "Output.h"
#include "ActionRecord.h"
#include "CommitSession.h"
#include "Players.h"
#include "Map.h"
#include "Race.h"
#include "CombatCmd.h"
#include "StartBattleRecord.h"
#include "FinishBattleRecord.h"
#include "AttackShipsRecord.h"
#include "AttackPopulationRecord.h"
#include "AdvanceCombatTurnRecord.h"

CombatPhase::CombatPhase(LiveGame* pGame) : OrderedPhase(pGame), m_lastPopulationBattleHexId(0)
{
}

CombatPhase::~CombatPhase()
{
}

void CombatPhase::Init(CommitSession& session)
{
	StartBattle(session);
}

const Hex* CombatPhase::GetNextBattleHex() const
{
	return GetGame().GetMap().FindPendingBattleHex(GetGame(), m_lastPopulationBattleHexId);
}

void CombatPhase::StartBattle(CommitSession& session, const Battle* oldBattle)
{
	auto hex = GetNextBattleHex();
	VERIFY(hex != nullptr);
	session.DoAndPushRecord(RecordPtr(new StartBattleRecord(oldBattle, hex->GetID())));
	
	if (RecordPtr rec = GetGame().GetBattle().CreateAutoAttackRecord(GetGame()))
	{
		session.DoAndPushRecord(std::move(rec));
		FinishBattle(session);
	}
	else
		StartTurn(session);
}

void CombatPhase::StartTurn(CommitSession& session)
{
	Colour colour = GetGame().GetBattle().GetFiringColour();
	if (colour == Colour::None)
	{
		Dice dice;
		GetGame().GetBattle().RollDice(session.GetGame(), dice);
		session.DoAndPushRecord(session.GetGame().GetBattle().CreateAttackRecord(session.GetGame(), dice));

		// TODO: Send attack animation 

		FinishTurn(session);
	}
	else
		StartCmd(CmdPtr(new CombatCmd(GetGame().GetBattle().GetFiringColour(), GetGame())), session);
}

void CombatPhase::OnCmdFinished(const Cmd& cmd, CommitSession& session)
{
	FinishTurn(session);
}

void CombatPhase::FinishTurn(CommitSession& session)
{
	session.DoAndPushRecord(RecordPtr(new AdvanceCombatTurnRecord()));
	
	auto& battle = GetGame().GetBattle();
	if (battle.IsFinished())
		FinishBattle(session);
	else
		StartTurn(session);
}

void CombatPhase::FinishBattle(CommitSession& session)
{
	auto& battle = GetGame().GetBattle();
	VERIFY_MODEL(battle.IsFinished());

	if (battle.IsPopulationBattle())
		m_lastPopulationBattleHexId = battle.GetHexId();
	
	BattlePtr prevHexBattle;
	auto hex = GetNextBattleHex();
	if (hex && hex->GetID() == GetGame().GetBattle().GetHexId()) // Continue battles in this hex.
	{
		prevHexBattle = GetGame().GetBattle().Clone();
	}
	else
	{
		// TODO: assign reputation tiles.
	}

	session.DoAndPushRecord(RecordPtr(new FinishBattleRecord));

	if (hex)
		StartBattle(session, prevHexBattle.get());
	else
	{
		LiveGame& game = GetGame();
		game.FinishCombatPhase(); // Deletes this. TODO: influence hexes.
		game.GetPhase().UpdateClient(session.GetController(), nullptr); // Show next phase UI (upkeep).
	}
}

void CombatPhase::UpdateClient(const Controller& controller, const Player* pPlayer) const
{
	if (const Cmd* pCmd = GetCurrentCmd())
		pCmd->UpdateClient(controller, GetGame());

	__super::UpdateClient(controller, pPlayer);
}

const Team& CombatPhase::GetCurrentTeam() const
{
	Colour colour = GetGame().GetBattle().GetFiringColour();
	
	//if (colour == Colour::None) // Keep OrderedPhase::FinishCmd happy.
	//	colour = GetGame().GetBattle().GetTargetColour();

	return GetGame().GetTeam(colour);
}

void CombatPhase::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveType("last_population_battle_hex_id", m_lastPopulationBattleHexId);
}

void CombatPhase::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadType("last_population_battle_hex_id", m_lastPopulationBattleHexId);
}

REGISTER_DYNAMIC(CombatPhase)
