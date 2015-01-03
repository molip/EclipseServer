#include "stdafx.h"
#include "OrderedPhase.h"
#include "CmdStack.h"
#include "LiveGame.h"
#include "Controller.h"
#include "Output.h"
#include "ActionRecord.h"
#include "CommitSession.h"
#include "Players.h"

OrderedPhase::OrderedPhase(LiveGame* pGame) : Phase(pGame)
{
	m_pCmdStack = new CmdStack;
}

OrderedPhase::~OrderedPhase()
{
	delete m_pCmdStack;
}

CmdStack& OrderedPhase::GetCmdStack(Colour c)
{
	VERIFY_MODEL(IsTeamActive(c));
	return *m_pCmdStack;
}

bool OrderedPhase::IsTeamActive(Colour c) const 
{
	return c == GetCurrentTeam().GetColour();
}

const Player& OrderedPhase::GetCurrentPlayer() const
{
	return Players::Get(GetCurrentTeam().GetPlayerID());
}

Cmd* OrderedPhase::GetCurrentCmd()
{
	return m_pCmdStack->GetCurrentCmd();
}

bool OrderedPhase::CanRemoveCmd() const
{
	return m_pCmdStack->CanRemoveCmd();
}

void OrderedPhase::StartCmd(CmdPtr pCmd, CommitSession& session)
{
	VERIFY_MODEL_MSG("Team not active", IsTeamActive(pCmd->GetColour()));

	m_pCmdStack->StartCmd(pCmd);

	Cmd* pStartedCmd = GetCurrentCmd();

	if (pStartedCmd->IsAutoProcess())
		ProcessCmdMessage(Input::CmdMessage(), session, GetCurrentPlayer());
	else
		pStartedCmd->UpdateClient(session.GetController(), GetGame());
}

void OrderedPhase::Save(Serial::SaveNode& node) const 
{
	Phase::Save(node);
	node.SaveClass("commands", *m_pCmdStack);
}

void OrderedPhase::Load(const Serial::LoadNode& node)
{
	Phase::Load(node);
	node.LoadClass("commands", *m_pCmdStack);
}
