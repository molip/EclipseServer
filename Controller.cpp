#include "Controller.h"
#include "Model.h"
#include "WSServer.h"
#include "Output.h"
#include "Input.h"
#include "Player.h"

#include "App.h"

SINGLETON(Controller)

Controller::Controller(Model& model) : m_model(model)
{
	model.SetController(this);
}

void Controller::SendUpdateGameList(const Player* pPlayer) const
{
	Output::UpdateGameList msg(m_model);
	if (pPlayer)
		m_pServer->SendMessage(msg, *pPlayer);
	else
		m_pServer->BroadcastMessage(msg);
}

void Controller::OnMessage(const Input::MessagePtr& pMsg, Player& player)
{
	bool bOK = pMsg->Process(*this, player);
	ASSERT(bOK);
}

bool Controller::SendMessage(const Output::Message& msg, const Player& player) const
{
	return m_pServer->SendMessage(msg, player);
}

bool Controller::SendMessage(const Output::Message& msg, const Game& game, const Player* pPlayer) const
{
	std::string str = msg.GetXML();
	if (!pPlayer)
	{
		for (auto& t : game.GetTeams())
			if (t.first->GetCurrentGame() == &game)
				m_pServer->SendMessage(str, *t.first);
	}
	else
	{
		AssertThrow("Controller::SendMessage: Player not in game: " + pPlayer->GetName(), pPlayer->GetCurrentGame() == &game);
		m_pServer->SendMessage(str, *pPlayer);
	}
	return true;
}

void Controller::OnPlayerConnected(Player& player)
{
	if (Game* pGame = player.GetCurrentGame())
		SendUpdateGame(*pGame, &player);
	else
	{
		m_pServer->SendMessage(Output::ShowGameList(), player);
		SendUpdateGameList(&player);
	}
}

void Controller::OnPlayerDisconnected(Player& player)
{

}

// Called from: Connect, join game, start game, choose team.
void Controller::SendUpdateGame(const Game& game, const Player* pPlayer) const
{
	bool bSendToCurrentPlayer = game.HasStarted() && (!pPlayer || pPlayer == &game.GetCurrentPlayer());

	if (game.GetPhase() == Game::Phase::Lobby)
	{
		SendMessage(Output::UpdateLobby(game), game, pPlayer);

		if (pPlayer)
		{
			SendMessage(Output::ShowLobby(), *pPlayer);
			SendMessage(Output::UpdateLobbyControls(pPlayer == &game.GetOwner()), *pPlayer);
		}
	}
	else if (game.GetPhase() == Game::Phase::ChooseTeam)
	{
		SendMessage(Output::ShowChoose(), game, pPlayer);
		SendMessage(Output::UpdateChoose(game), game, pPlayer);

		if (bSendToCurrentPlayer)
			SendMessage(Output::ChooseTeam(game, true), game.GetCurrentPlayer());
	}
	else if(game.GetPhase() == Game::Phase::Main)
	{
		SendMessage(Output::ShowGame(), game, pPlayer);
		SendMessage(Output::UpdateTeams(game), game, pPlayer);
	
		for (auto& team : game.GetTeams())
		{
			const Team* pTeam = team.second.get();
			AssertThrow("Controller::SendUpdateGame: Team not chosen yet: " + team.first->GetName(), !!pTeam);
			SendMessage(Output::UpdateTeam(*pTeam), game, pPlayer);
			SendMessage(Output::UpdateInfluenceTrack(*pTeam), game, pPlayer);
		}
		SendMessage(Output::UpdateMap(game), game, pPlayer);

		if (bSendToCurrentPlayer) 
			if (const Cmd* pCmd = game.GetCurrentCmd())
				pCmd->UpdateClient(*this);
			else
				SendMessage(Output::ChooseAction(game), game.GetCurrentPlayer());
	}
}
