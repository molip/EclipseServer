#include "stdafx.h"
#include "Phase.h"
#include "CmdStack.h"
#include "Players.h"
#include "LiveGame.h"
#include "Controller.h"
#include "Output.h"
#include "Games.h"
#include "ReviewGame.h"

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

void Phase::SaveGame()
{
	m_pGame->Save();
}

void Phase::ProcessCmdMessage(const Input::CmdMessage& msg, Controller& controller, Player& player) 
{
	LiveGame& game = GetGame();
	AssertThrow("Phase::ProcessCmdMessage: Player not in game", &game == player.GetCurrentLiveGame());

	const Colour colour = game.GetTeam(player).GetColour();
	Cmd* pCmd = GetCurrentCmd(colour);
	AssertThrow("Phase::ProcessCmdMessage: No current command", !!pCmd);

	CmdPtr pNext = pCmd->Process(msg, controller, game); // Might be null.
	if (pNext)
		AddCmd(std::move(pNext)); 
	else
		FinishCmd(colour);

	UpdateClient(controller, &player);

	if (pCmd->HasRecord()) 
		for (auto& g : Games::GetReviewGames())
			if (g->GetLiveGameID() == game.GetID())
				controller.SendMessage(Output::UpdateReviewUI(*g), *g);
}

void Phase::UndoCmd(Controller& controller, Player& player) 
{
	LiveGame& game = GetGame();
	AssertThrow("Phase::UndoCmd: Player not in game", &game == player.GetCurrentLiveGame());

	const Colour colour = game.GetTeam(player).GetColour();
	const Cmd* pCmd = GetCurrentCmd(colour);

	if (Cmd* pUndo = RemoveCmd(controller, colour))
	{
		if (pUndo->HasRecord())
		{
			// Update review games before record gets popped. 
			for (auto& g : Games::GetReviewGames())
				if (g->GetLiveGameID() == game.GetID())
					g->OnPreRecordPop(controller);

			pUndo->PopRecord(controller, game);

			for (auto& g : Games::GetReviewGames())
				if (g->GetLiveGameID() == game.GetID())
					controller.SendMessage(Output::UpdateReviewUI(*g), *g);
		}

		if (pUndo->IsAutoProcess()) // Also undo the command start. 
			RemoveCmd(controller, colour);
	}

	UpdateClient(controller, &player);
}

//-----------------------------------------------------------------------------

TurnPhase::TurnPhase(LiveGame* pGame) : Phase(pGame), m_iTurn(0)
{
}

const Player& TurnPhase::GetCurrentPlayer() const
{
	return Players::Get(GetCurrentTeam().GetPlayerID());
}

Player& TurnPhase::GetCurrentPlayer() 
{
	return Players::Get(GetCurrentTeam().GetPlayerID());
}

Team& TurnPhase::GetCurrentTeam()
{
	return *GetGame().GetTeams()[m_iTurn % GetGame().GetTeams().size()];
}

void TurnPhase::AdvanceTurn()
{
	++m_iTurn;
	SaveGame();
}

void TurnPhase::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveType("turn", m_iTurn);
}

void TurnPhase::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadType("turn", m_iTurn);
}

//-----------------------------------------------------------------------------
