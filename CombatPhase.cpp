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

CombatPhase::CombatPhase(LiveGame* pGame, const HexArrivals& hexArrivals) : 
OrderedPhase(pGame), m_hexArrivals(hexArrivals)
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
	if (m_hexArrivals.empty())
		return false;

	auto it = m_hexArrivals.rbegin();
	auto& site = it->second;
	VerifyModel("CombatPhase::StartBattle 1", site.size() >= 2);

	const Hex* hex = GetGame().GetMap().FindHex(it->first);
	VerifyModel("CombatPhase::StartBattle 2", hex != nullptr);

	if (site.IsPeaceful(GetGame()))
	{
		m_hexArrivals.erase(--m_hexArrivals.end());
		// TODO: Hex battles finished: attack population, take rep tiles.  
		return StartBattle();
	}

	m_battle.reset(new Battle(*hex, site[site.size() - 2], site[site.size() - 1], GetGame()));

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
	node.SaveClass("hex_arrivals", m_hexArrivals);
	node.SaveClassPtr("battle", m_battle);
}

void CombatPhase::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadClass("hex_arrivals", m_hexArrivals);
	node.LoadClassPtr("battle", m_battle);
}

REGISTER_DYNAMIC(CombatPhase)
