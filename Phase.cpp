#include "stdafx.h"
#include "Phase.h"
#include "CmdStack.h"
#include "Players.h"
#include "LiveGame.h"
#include "Controller.h"
#include "Output.h"
#include "Games.h"
#include "ReviewGame.h"
#include "CommitSession.h"

Phase::Phase(LiveGame* pGame) : m_pGame(pGame)
{

}

void Phase::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
}

void Phase::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
}

void Phase::ProcessCmdMessage(const Input::CmdMessage& msg, CommitSession& session, const Player& player)
{
	LiveGame& game = GetGame();
	VerifyModel("Phase::ProcessCmdMessage: Player not in game", &game == player.GetCurrentLiveGame());

	const Colour colour = game.GetTeam(player).GetColour();
	Cmd* pCmd = GetCurrentCmd(colour);
	VerifyModel("Phase::ProcessCmdMessage: No current command", !!pCmd);

	CmdPtr pNext = pCmd->Process(msg, session); // Might be null.
	if (pNext)
		AddCmd(std::move(pNext)); 
	else
		FinishCmd(colour);

	UpdateClient(session.GetController(), &player);

	if (pCmd->HasRecord()) 
		session.UpdateReviewGames();
}

void Phase::UndoCmd(CommitSession& session, Player& player)
{
	LiveGame& game = GetGame();
	VerifyModel("Phase::UndoCmd: Player not in game", &game == player.GetCurrentLiveGame());

	const Colour colour = game.GetTeam(player).GetColour();
	const Cmd* pCmd = GetCurrentCmd(colour);

	const Controller& controller = session.GetController();
	if (Cmd* pUndo = RemoveCmd(session, colour))
	{
		if (pUndo->HasRecord())
		{
			// Update review games before record gets popped. 
			for (auto& g : game.GetReviewGames())
				g->OnPreRecordPop(controller);

			pUndo->PopRecord(session);

			session.UpdateReviewGames();
		}

		if (pUndo->IsAutoProcess()) // Also undo the command start. 
			RemoveCmd(session, colour);
	}

	UpdateClient(controller, &player);
}

//-----------------------------------------------------------------------------

TurnPhase::TurnPhase() : m_iTurn(0)
{
}

const Team& TurnPhase::GetCurrentTeam(const LiveGame& game) const
{
	return *game.GetTeams()[m_iTurn % game.GetTeams().size()];
}

void TurnPhase::AdvanceTurn()
{
	++m_iTurn;
}

void TurnPhase::Save(Serial::SaveNode& node) const 
{
	node.SaveType("turn", m_iTurn);
}

void TurnPhase::Load(const Serial::LoadNode& node)
{
	node.LoadType("turn", m_iTurn);
}

//-----------------------------------------------------------------------------

