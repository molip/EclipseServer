#include "stdafx.h"
#include "ActionPhase.h"
#include "CmdStack.h"
#include "LiveGame.h"
#include "Controller.h"
#include "Output.h"
#include "ActionRecord.h"
#include "CommitSession.h"

ActionPhase::ActionPhase(LiveGame* pGame) : OrderedPhase(pGame), m_bDoneAction(false)
{
}

ActionPhase::~ActionPhase()
{
}

const Team& ActionPhase::GetCurrentTeam() const 
{
	return TurnPhase::GetCurrentTeam(GetGame());
}

Cmd* ActionPhase::RemoveCmd(CommitSession& session, Colour c)
{
	const Cmd* pCmd = GetCurrentCmd(c);
	bool bAction = pCmd && pCmd->IsAction();
	bool bCostsInfluence = pCmd && pCmd->CostsInfluence();

	Cmd* pUndo = OrderedPhase::RemoveCmd(session, c);

	if (bAction && !pUndo) // It's a start cmd.
	{
		VERIFY_MODEL(m_bDoneAction);
		m_bDoneAction = false;

		if (bCostsInfluence)
		{
			RecordPtr pRec = session.PopAndUndoRecord();
			VERIFY_MODEL_MSG("current record not ActionRecord", !!dynamic_cast<ActionRecord*>(pRec.get()));
		}
	}

	return pUndo;
}

void ActionPhase::StartCmd(CmdPtr pCmd, CommitSession& session)
{
	VERIFY_MODEL_MSG("Team not active", IsTeamActive(pCmd->GetColour()));

	if (pCmd->IsAction()) // Includes PassCmd.
	{
		VERIFY_MODEL_MSG("Already done an action", !m_bDoneAction);
		m_bDoneAction = true;
	}

	if (pCmd->CostsInfluence())
	{
		session.DoAndPushRecord(RecordPtr(new ActionRecord(pCmd->GetActionName(), GetCurrentTeam().GetColour())));
	}
	
	OrderedPhase::StartCmd(std::move(pCmd), session);
}

bool ActionPhase::CanDoAction() const
{
	if (GetCurrentTeam().GetInfluenceTrack().GetDiscCount() == 0)
		return false;
	
	return !m_bDoneAction; // Only one action per turn.
}

void ActionPhase::FinishTurn(CommitSession& session)
{
	const Controller& controller = session.GetController();
	controller.SendMessage(Output::ChooseFinished(), GetCurrentPlayer());

	Colour c = GetCurrentTeam().GetColour();
	if (GetCurrentTeam().HasPassed() && std::find(m_passOrder.begin(), m_passOrder.end(), c) == m_passOrder.end())
	{
		LiveGame& game = GetGame();

		m_passOrder.push_back(c);
		if (m_passOrder.size() == game.GetTeams().size())
		{
			game.FinishActionPhase(m_passOrder); // Deletes this.
			game.GetPhase().Init(session);
			game.GetPhase().UpdateClient(controller, nullptr); // Show next phase UI (combat or upkeep).
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
	OrderedPhase::Save(node);
	TurnPhase::Save(node);
	node.SaveType("done_action", m_bDoneAction);
	node.SaveCntr("pass_order", m_passOrder, Serial::EnumSaver());
}

void ActionPhase::Load(const Serial::LoadNode& node)
{
	OrderedPhase::Load(node);
	TurnPhase::Load(node);
	node.LoadType("done_action", m_bDoneAction);
	node.LoadCntr("pass_order", m_passOrder, Serial::EnumLoader());
}

REGISTER_DYNAMIC(ActionPhase)
