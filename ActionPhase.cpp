#include "stdafx.h"
#include "ActionPhase.h"
#include "CmdStack.h"
#include "LiveGame.h"
#include "Controller.h"
#include "Output.h"

ActionPhase::ActionPhase() : m_bDoneAction(false)
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

Cmd* ActionPhase::RemoveCmd(Colour c)
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
			GetCurrentTeam().GetInfluenceTrack().AddDiscs(1);
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
	AssertThrow("ActionPhase::StartCmd", IsTeamActive(pCmd->GetColour()));

	if (pCmd->IsAction()) // Includes PassCmd.
	{
		AssertThrow("Phase::StartCmd",  !m_bDoneAction);
		m_bDoneAction = true;
	}

	if (pCmd->CostsInfluence())
		GetCurrentTeam().GetInfluenceTrack().RemoveDiscs(1); // TODO: return these at end of 

	m_pCmdStack->StartCmd(pCmd);

	SaveGame();

	Cmd* pStartedCmd = GetCurrentCmd();

	if (pStartedCmd->CostsInfluence())
		controller.SendMessage(Output::UpdateInfluenceTrack(GetCurrentTeam()), GetGame()); // TODO: Move to record.

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

	m_pCmdStack->Clear();
	m_bDoneAction = false;
	AdvanceTurn();
	SaveGame();

	controller.SendMessage(Output::ChooseAction(GetGame()), GetCurrentPlayer());
}

void ActionPhase::UpdateClient(const Controller& controller) const 
{
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
}

void ActionPhase::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadClass("commands", *m_pCmdStack);
	node.LoadType("done_action", m_bDoneAction);
}

REGISTER_DYNAMIC(ActionPhase)
