#include "stdafx.h"
#include "ActionPhase.h"
#include "CmdStack.h"
#include "LiveGame.h"
#include "Controller.h"
#include "Output.h"
#include "ActionRecord.h"

ActionPhase::ActionPhase(LiveGame* pGame) : TurnPhase(pGame), m_bDoneAction(false)
{
	m_pCmdStack = new CmdStack;
}

ActionPhase::~ActionPhase()
{
	delete m_pCmdStack;
}

void ActionPhase::AddCmd(CmdPtr pCmd)
{
	AssertThrow("ActionPhase::AddCmd: Team not active", IsTeamActive(pCmd->GetColour()));
	AssertThrow("ActionPhase::AddCmd: pCmd is null",  !!pCmd);

	m_pCmdStack->AddCmd(pCmd);
	SaveGame();
}

void ActionPhase::FinishCmd(Colour c)
{
	AssertThrow("ActionPhase::FinishCmd", IsTeamActive(c));

	m_pCmdStack->AddCmd(CmdPtr());
	SaveGame();
}

Cmd* ActionPhase::RemoveCmd(const Controller& controller, Colour c)
{
	AssertThrow("ActionPhase::RemoveCmd", IsTeamActive(c));

	const Cmd* pCmd = GetCurrentCmd(c);
	bool bAction = pCmd && pCmd->IsAction();
	bool bCostsInfluence = pCmd && pCmd->CostsInfluence();

	Cmd* pUndo = m_pCmdStack->RemoveCmd();

	if (bAction && !pUndo) // It's a start cmd.
	{
		AssertThrow("Phase::RemoveCmd",  m_bDoneAction);
		m_bDoneAction = false;

		if (bCostsInfluence)
		{
			RecordPtr pRec = GetGame().PopRecord();
			AssertThrow("ActionPhase::RemoveCmd: current record not ActionRecord", !!dynamic_cast<ActionRecord*>(pRec.get()));
			pRec->Undo(GetGame(), controller);
		}
	}

	SaveGame();
	return pUndo;
}

bool ActionPhase::CanRemoveCmd(Colour c) const
{
	AssertThrow("ActionPhase::CanRemoveCmd", IsTeamActive(c));
	return CanRemoveCmd();
}

bool ActionPhase::IsTeamActive(Colour c) const 
{
	return c == GetCurrentTeam().GetColour();
}

Cmd* ActionPhase::GetCurrentCmd(Colour c)
{
	AssertThrow("ActionPhase::GetCurrentCmd", IsTeamActive(c));
	return GetCurrentCmd();
}

Cmd* ActionPhase::GetCurrentCmd()
{
	return m_pCmdStack->GetCurrentCmd();
}

bool ActionPhase::CanRemoveCmd() const
{
	return m_pCmdStack->CanRemoveCmd();
}

void ActionPhase::StartCmd(CmdPtr pCmd, Controller& controller)
{
	AssertThrow("ActionPhase::StartCmd: Team not active", IsTeamActive(pCmd->GetColour()));

	if (pCmd->IsAction()) // Includes PassCmd.
	{
		AssertThrow("ActionPhasePhase::StartCmd: Already done an action",  !m_bDoneAction);
		m_bDoneAction = true;
	}

	if (pCmd->CostsInfluence())
	{
		RecordPtr pRec = std::make_unique<ActionRecord>(GetCurrentTeam().GetColour());
		pRec->Do(GetGame(), controller);
		GetGame().PushRecord(pRec);
	}
	
	m_pCmdStack->StartCmd(pCmd);

	SaveGame();

	Cmd* pStartedCmd = GetCurrentCmd();

	if (pStartedCmd->IsAutoProcess())
		ProcessCmdMessage(Input::CmdMessage(), controller, GetCurrentPlayer());
	else
		pStartedCmd->UpdateClient(controller, GetGame());
}

bool ActionPhase::CanDoAction() const
{
	if (GetCurrentTeam().GetInfluenceTrack().GetDiscCount() == 0)
		return false;
	
	return !m_bDoneAction; // Only one action per turn.
}

void ActionPhase::FinishTurn(Controller& controller)
{
	controller.SendMessage(Output::ChooseFinished(), GetCurrentPlayer());

	Colour c = GetCurrentTeam().GetColour();
	if (GetCurrentTeam().HasPassed() && std::find(m_passOrder.begin(), m_passOrder.end(), c) == m_passOrder.end())
	{
		LiveGame& game = GetGame();

		m_passOrder.push_back(c);
		if (m_passOrder.size() == game.GetTeams().size())
		{
			game.FinishActionPhase(m_passOrder); // Deletes this.
			game.GetPhase().UpdateClient(controller, nullptr); // Show upkeep UI.
			return;
		}
	}
	m_pCmdStack->Clear();
	m_bDoneAction = false;
	AdvanceTurn();

	UpdateClient(controller, &GetCurrentPlayer());
}

void ActionPhase::UpdateClient(const Controller& controller, const Player* pPlayer) const
{
	if (pPlayer && pPlayer != &GetCurrentPlayer())
		return; // Nothing to send to this player.

	if (const Cmd* pCmd = GetCurrentCmd())
		pCmd->UpdateClient(controller, GetGame());
	else
		controller.SendMessage(Output::ChooseAction(GetGame()), GetCurrentPlayer());
}

void ActionPhase::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveClass("commands", *m_pCmdStack);
	node.SaveType("done_action", m_bDoneAction);
	node.SaveCntr("pass_order", m_passOrder, Serial::EnumSaver());
}

void ActionPhase::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadClass("commands", *m_pCmdStack);
	node.LoadType("done_action", m_bDoneAction);
	node.LoadCntr("pass_order", m_passOrder, Serial::EnumLoader());
}

REGISTER_DYNAMIC(ActionPhase)
