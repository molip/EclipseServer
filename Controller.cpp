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
	
	if (pLive && !pLive->HasStarted())
	{
		SendMessage(Output::UpdateLobby(game), game, pPlayer);

		if (pPlayer)
		{
			SendMessage(Output::ShowLobby(), *pPlayer);
			SendMessage(Output::UpdateLobbyControls(pPlayer == &game.GetOwner()), *pPlayer);
		}
		return;
	}

	if (!pLive || pLive->GetGamePhase() == LiveGame::GamePhase::Main)
	{
		SendMessage(Output::ShowGame(), game, pPlayer);
		SendMessage(Output::UpdateTeams(game), game, pPlayer);

		if (auto pReview = dynamic_cast<const ReviewGame*>(&game))
			SendMessage(Output::UpdateReviewUI(*pReview), game, pPlayer);

		// Send info about each team.
		for (auto& pInfoTeam : game.GetTeams())
		{
			const Player& infoPlayer = pInfoTeam->GetPlayer();
			AssertThrow("Controller::SendUpdateGame: Team not chosen yet: " + infoPlayer.GetName(), !!pInfoTeam);
			SendMessage(Output::UpdateTeam(*pInfoTeam), game, pPlayer);
			SendMessage(Output::UpdateInfluenceTrack(*pInfoTeam), game, pPlayer);
			SendMessage(Output::UpdateTechnologyTrack(*pInfoTeam), game, pPlayer);
			SendMessage(Output::UpdateStorageTrack(*pInfoTeam), game, pPlayer);
			SendMessage(Output::UpdatePopulationTrack(*pInfoTeam), game, pPlayer);
			SendMessage(Output::UpdatePassed(*pInfoTeam), game, pPlayer);

			// Reputation tile values are secret, so only send them to the relevant player. 
			for (auto& pDstTeam : game.GetTeams())
			{
				const Player& dstPlayer = pDstTeam->GetPlayer();
				if (dstPlayer.GetCurrentGame() == &game)
					if (!pPlayer || pPlayer == &dstPlayer)
						SendMessage(Output::UpdateReputationTrack(*pInfoTeam, &dstPlayer == &infoPlayer), game, &dstPlayer);
			}
		}
		SendMessage(Output::UpdateMap(game), game, pPlayer);
		SendMessage(Output::UpdateTechnologies(game), game, pPlayer);
		SendMessage(Output::UpdateRound(game), game, pPlayer);
	}

	if (pLive)
		pLive->GetPhase().UpdateClient(*this, pPlayer);
}
