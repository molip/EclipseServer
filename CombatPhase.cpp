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
#include "AttackRecord.h"
#include "AdvanceCombatTurnRecord.h"

CombatPhase::CombatPhase() : OrderedPhase(nullptr)
{
}

CombatPhase::CombatPhase(LiveGame* pGame) : 
OrderedPhase(pGame)
{
}

CombatPhase::~CombatPhase()
{
}

void CombatPhase::Init(CommitSession& session)
{
	StartBattle(session);
}

void CombatPhase::StartBattle(CommitSession& session, const Battle* oldBattle)
{
	session.DoAndPushRecord(RecordPtr(new StartBattleRecord(oldBattle)));
	StartTurn(session);
}

void CombatPhase::StartTurn(CommitSession& session)
{
	Colour colour = GetGame().GetBattle().GetFiringColour();
	if (colour == Colour::None)
	{
		Dice dice;
		GetGame().GetBattle().RollDice(session.GetGame(), dice);
		const Battle::Hits hits = GetGame().GetBattle().AutoAssignHits(dice, session.GetGame());

		// TODO: Send attack animation 

		session.DoAndPushRecord(RecordPtr(new AttackRecord(hits)));
		FinishTurn(session);
	}
	else
		StartCmd(CmdPtr(new CombatCmd(GetGame().GetBattle().GetFiringColour(), GetGame())), session);
}

void CombatPhase::FinishCmd(CommitSession& session, Colour c)
{
	OrderedPhase::FinishCmd(session, c);
	FinishTurn(session);
}

void CombatPhase::FinishTurn(CommitSession& session)
{
	if (GetGame().GetBattle().IsFinished())
		FinishBattle(session);
	else
	{
		session.DoAndPushRecord(RecordPtr(new AdvanceCombatTurnRecord()));
		StartTurn(session);
	}
}

void CombatPhase::FinishBattle(CommitSession& session)
{
	const Battle* oldBattle = nullptr;
	
	auto hex = GetGame().GetMap().FindPendingBattleHex(GetGame());
	if (hex && hex->GetID() == GetGame().GetBattle().GetHexId()) // Continue battles in this hex.
	{
		oldBattle = &GetGame().GetBattle();
	}
	else
	{
		// TODO: Attack population, assign reputation tiles.
	}

	session.DoAndPushRecord(RecordPtr(new FinishBattleRecord));

	if (hex)
		StartBattle(session, oldBattle);
	else
		GetGame().FinishCombatPhase(); // Deletes this. TODO: influence hexes.
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
}

void CombatPhase::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
}

REGISTER_DYNAMIC(CombatPhase)
