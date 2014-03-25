#include "stdafx.h"
#include "ActionPhase.h"
#include "CmdStack.h"
#include "LiveGame.h"
#include "Controller.h"
#include "Output.h"
#include "ActionRecord.h"
#include "CommitSession.h"

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
	VerifyModel("ActionPhase::AddCmd: Team not active", IsTeamActive(pCmd->GetColour()));
	VerifyModel("ActionPhase::AddCmd: pCmd is null", !!pCmd);

	m_pCmdStack->AddCmd(pCmd);
}

void ActionPhase::FinishCmd(Colour c)
{
	VerifyModel("ActionPhase::FinishCmd", IsTeamActive(c));

	m_pCmdStack->AddCmd(CmdPtr());
}

Cmd* ActionPhase::RemoveCmd(CommitSession& session, Colour c)
{
	VerifyModel("ActionPhase::RemoveCmd", IsTeamActive(c));

	const Cmd* pCmd = GetCurrentCmd(c);
	bool bAction = pCmd && pCmd->IsAction();
	bool bCostsInfluence = pCmd && pCmd->CostsInfluence();

	Cmd* pUndo = m_pCmdStack->RemoveCmd();

	if (bAction && !pUndo) // It's a start cmd.
	{
		VerifyModel("Phase::RemoveCmd", m_bDoneAction);
		m_bDoneAction = false;

		if (bCostsInfluence)
		{
			RecordPtr pRec = session.PopAndUndoRecord();
			VerifyModel("ActionPhase::RemoveCmd: current record not ActionRecord", !!dynamic_cast<ActionRecord*>(pRec.get()));

			session.UpdateReviewGames();
		}
	}

	return pUndo;
}

bool ActionPhase::CanRemoveCmd(Colour c) const
{
	VerifyModel("ActionPhase::CanRemoveCmd", IsTeamActive(c));
	return CanRemoveCmd();
}

bool ActionPhase::IsTeamActive(Colour c) const 
{
	return c == GetCurrentTeam().GetColour();
}

Cmd* ActionPhase::GetCurrentCmd(Colour c)
{
	VerifyModel("ActionPhase::GetCurrentCmd", IsTeamActive(c));
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

void ActionPhase::StartCmd(CmdPtr pCmd, CommitSession& session)
{
	VerifyModel("ActionPhase::StartCmd: Team not active", IsTeamActive(pCmd->GetColour()));

	if (pCmd->IsAction()) // Includes PassCmd.
	{
		VerifyModel("ActionPhase::StartCmd: Already done an action", !m_bDoneAction);
		m_bDoneAction = true;
	}

	const Controller& controller = session.GetController();

	if (pCmd->CostsInfluence())
	{
		session.DoAndPushRecord(RecordPtr(new ActionRecord(pCmd->GetActionName(), GetCurrentTeam().GetColour())));
		session.UpdateReviewGames();
	}
	
	m_pCmdStack->StartCmd(pCmd);

	Cmd* pStartedCmd = GetCurrentCmd();

	if (pStartedCmd->IsAutoProcess())
		ProcessCmdMessage(Input::CmdMessage(), session, GetCurrentPlayer());
	else
		pStartedCmd->UpdateClient(controller, GetGame());
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
			game.FinishActionPhase(m_passOrder, m_hexArrivalOrder); // Deletes this.
			game.GetPhase().UpdateClient(controller, nullptr); // Show next phase UI (combat or upkeep).
			return;
		}
	}
	m_pCmdStack->Clear();
	m_bDoneAction = false;
	AdvanceTurn();

	UpdateClient(controller, &GetCurrentPlayer());
}

void ActionPhase::ShipMovedFrom(const Hex& hex, Colour colour)
{
	if (!hex.HasShip(colour, false)) // The last ship of /colour/ has left.
	{
		auto itMap = m_hexArrivalOrder.find(hex.GetID());
		if (itMap != m_hexArrivalOrder.end()) // Hex is/was contended.
		{
			// This should only happen when undoing a move/build record.
			// Otherwise the ship should have been pinned. 
			
			auto& vec = itMap->second;
			auto itVec = std::find(vec.begin(), vec.end(), colour);
			VerifyModel("ActionPhase::ShipMovedFrom 1", itVec != vec.end());
			
			vec.erase(itVec);
			VerifyModel("ActionPhase::ShipMovedFrom 2", std::find(vec.begin(), vec.end(), colour) == vec.end());

			if (vec.size() == 1) // Forget hexes with only one colour ship.
			{
				VerifyModel("ActionPhase::ShipMovedFrom 3", !hex.HasForeignShip(vec.front(), true));
				m_hexArrivalOrder.erase(itMap);
			}
		}
	}
}

void ActionPhase::ShipMovedTo(const Hex& hex, Colour colour)
{
	std::set<Colour> colours = hex.GetShipColours(true);
	VerifyModel("ActionPhase::ShipMovedTo 1", colours.erase(colour) == 1);

	if (colours.empty()) // No contention.
		return;

	auto& vec = m_hexArrivalOrder[hex.GetID()];
	if (vec.empty()) // Add the colour that was there first.
	{
		VerifyModel("ActionPhase::ShipMovedTo 2", colours.size() == 1); // Should already be in vec!
		vec.push_back(*colours.begin());
	}
	if (std::find(vec.begin(), vec.end(), colour) == vec.end())
		vec.push_back(colour);
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
