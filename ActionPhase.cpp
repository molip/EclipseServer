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

void ActionPhase::StartCmd(CmdPtr pCmd, CommitSession& session)
{
	VerifyModel("ActionPhase::StartCmd: Team not active", IsTeamActive(pCmd->GetColour()));

	if (pCmd->IsAction()) // Includes PassCmd.
	{
		VerifyModel("ActionPhase::StartCmd: Already done an action", !m_bDoneAction);
		m_bDoneAction = true;
	}

	if (pCmd->CostsInfluence())
	{
		session.DoAndPushRecord(RecordPtr(new ActionRecord(pCmd->GetActionName(), GetCurrentTeam().GetColour())));
		session.UpdateReviewGames();
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
			auto& vec = itMap->second;
			auto itVec = std::find(vec.begin(), vec.end(), colour);
			VerifyModel("ActionPhase::ShipMovedFrom 1", itVec != vec.end());
			
			vec.erase(itVec);
			VerifyModel("ActionPhase::ShipMovedFrom 2", std::find(vec.begin(), vec.end(), colour) == vec.end());

			if (vec.size() == 1) // Forget hexes with only one colour ship.
			{
				VerifyModel("ActionPhase::ShipMovedFrom 3", !hex.HasForeignShip(vec.front(), false));
				m_hexArrivalOrder.erase(itMap);
			}
		}
	}
}

void ActionPhase::ShipMovedTo(const Hex& hex, Colour colour)
{
	std::set<Colour> colours = hex.GetShipColours(false);
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
	OrderedPhase::Save(node);
	TurnPhase::Save(node);
	node.SaveType("done_action", m_bDoneAction);
	node.SaveCntr("pass_order", m_passOrder, Serial::EnumSaver());
	node.SaveClass("hex_arrivals", m_hexArrivalOrder);
}

void ActionPhase::Load(const Serial::LoadNode& node)
{
	OrderedPhase::Load(node);
	TurnPhase::Load(node);
	node.LoadType("done_action", m_bDoneAction);
	node.LoadCntr("pass_order", m_passOrder, Serial::EnumLoader());
	node.LoadClass("hex_arrivals", m_hexArrivalOrder);
}

REGISTER_DYNAMIC(ActionPhase)
