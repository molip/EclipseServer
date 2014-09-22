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
	VerifyModel("CombatPhase::Init", StartBattle());
	StartCmd(CmdPtr(new CombatCmd(m_battle->GetCurrentTeamColour(), GetGame())), session);
}

bool CombatPhase::StartBattle()
{
	const Hex* hex = GetGame().GetMap().FindPendingBattleHex(GetGame());

	if (!hex)
		return false;

	m_battle.reset(new Battle(*hex, GetGame()));

	return true;
}

void CombatPhase::FinishTurn(CommitSession& session)
{
}

void CombatPhase::UpdateClient(const Controller& controller, const Player* pPlayer) const
{
	controller.SendMessage(Output::UpdateShowCombat(GetGame(), true), GetGame(), pPlayer);
	controller.SendMessage(Output::UpdateCombat(GetGame(), *m_battle), GetGame(), pPlayer);

	if (const Cmd* pCmd = GetCurrentCmd())
		pCmd->UpdateClient(controller, GetGame());

	__super::UpdateClient(controller, pPlayer);
}

const Team& CombatPhase::GetCurrentTeam() const
{
	return GetGame().GetTeam(m_battle->GetCurrentTeamColour());
}

void CombatPhase::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveClassPtr("battle", m_battle);
}

void CombatPhase::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadClassPtr("battle", m_battle);
}

REGISTER_DYNAMIC(CombatPhase)
