#include "Battle.h"
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
	VerifyModel("CombatPhase::Init", StartBattle(session));
}

bool CombatPhase::StartBattle(CommitSession& session)
{
	if (!GetGame().GetMap().FindPendingBattleHex(GetGame()))
		return false;

	session.DoAndPushRecord(RecordPtr(new StartBattleRecord));

	StartCmd(CmdPtr(new CombatCmd(GetGame().GetBattle().GetCurrentTeamColour(), GetGame())), session);

	return true;
}

void CombatPhase::FinishTurn(CommitSession& session)
{
}

void CombatPhase::UpdateClient(const Controller& controller, const Player* pPlayer) const
{
	//controller.SendMessage(Output::UpdateShowCombat(GetGame(), true), GetGame(), pPlayer);
	//controller.SendMessage(Output::UpdateCombat(GetGame(), *m_battle), GetGame(), pPlayer);

	if (const Cmd* pCmd = GetCurrentCmd())
		pCmd->UpdateClient(controller, GetGame());

	__super::UpdateClient(controller, pPlayer);
}

const Team& CombatPhase::GetCurrentTeam() const
{
	return GetGame().GetTeam(GetGame().GetBattle().GetCurrentTeamColour());
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
