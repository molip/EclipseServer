#include "Controller.h"
#include "WSServer.h"
#include "Output.h"
#include "Input.h"
#include "Player.h"
#include "LiveGame.h"
#include "ReviewGame.h"
#include "ActionPhase.h"
#include "ChooseTeamPhase.h"

#include "App.h"

SINGLETON(Controller)

Controller::Controller()
{
}

void Controller::SendUpdateGameList(const Player* pPlayer) const
{
	Output::UpdateGameList msg;
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
			if (t->GetPlayer().GetCurrentGame() == &game)
				m_pServer->SendMessage(str, t->GetPlayer());
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

// Update everything. 
// pPlayer: if null, update all players. Otherwise just update pPlayer.
// Called from: Connect, join game, start game, choose team.
void Controller::SendUpdateGame(const Game& game, const Player* pPlayer) const
{
	auto pLive = dynamic_cast<const LiveGame*>(&game);
	
	if (pLive)
	{
		if (!pLive->HasStarted())
		{
			SendMessage(Output::UpdateLobby(game), game, pPlayer);

			if (pPlayer)
			{
				SendMessage(Output::ShowLobby(), *pPlayer);
				SendMessage(Output::UpdateLobbyControls(pPlayer == &game.GetOwner()), *pPlayer);
			}
			return;
		}
		if (const ChooseTeamPhase* pChooseTeamPhase = dynamic_cast<const ChooseTeamPhase*>(&pLive->GetPhase()))
		{
			SendMessage(Output::ShowChoose(), game, pPlayer);
			SendMessage(Output::UpdateChoose(*pLive), game, pPlayer);

			if (!pPlayer || pPlayer == &pChooseTeamPhase->GetCurrentPlayer())
				SendMessage(Output::ChooseTeam(game, true), pChooseTeamPhase->GetCurrentPlayer());
			return;
		}
	}

	SendMessage(Output::ShowGame(), game, pPlayer);
	SendMessage(Output::UpdateTeams(game), game, pPlayer);
	
	if (auto pReview = dynamic_cast<const ReviewGame*>(&game))
		SendMessage(Output::UpdateReviewUI(*pReview), game, pPlayer);
	
	for (auto& pTeam : game.GetTeams())
	{
		const Player& player = pTeam->GetPlayer();
		AssertThrow("Controller::SendUpdateGame: Team not chosen yet: " + player.GetName(), !!pTeam);
		SendMessage(Output::UpdateTeam(*pTeam), game, pPlayer);
		SendMessage(Output::UpdateInfluenceTrack(*pTeam), game, pPlayer);
		SendMessage(Output::UpdateTechnologyTrack(*pTeam), game, pPlayer);
		SendMessage(Output::UpdateStorageTrack(*pTeam), game, pPlayer);
		SendMessage(Output::UpdatePopulationTrack(*pTeam), game, pPlayer);
		SendMessage(Output::UpdatePassed(*pTeam), game, pPlayer);

		// Reputation tile values are secret, so only send them to the relevant player. 
		auto SendUpdateReputationTrack = [&] (const Player& player) { 
			SendMessage(Output::UpdateReputationTrack(*pTeam, &player == &player), game, &player); };

		if (pPlayer)
			SendUpdateReputationTrack(*pPlayer);
		else
			for (auto& t : game.GetTeams())
				if (player.GetCurrentGame() == &game)
					SendUpdateReputationTrack(player);
	}
	SendMessage(Output::UpdateMap(game), game, pPlayer);
	SendMessage(Output::UpdateTechnologies(game), game, pPlayer);

	if (pLive)
		if (const ActionPhase* pActionPhase = dynamic_cast<const ActionPhase*>(&pLive->GetPhase()))
			if (!pPlayer || pPlayer == &pActionPhase->GetCurrentPlayer()) 
				pActionPhase->UpdateClient(*this);
}
