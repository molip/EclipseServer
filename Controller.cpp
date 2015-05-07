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
	for (auto& player : m_pServer->GetPlayers())
		if (!pPlayer || pPlayer == player)
			SendMessage(std::make_shared<std::string>(Output::UpdateGameList(*player).GetXML()), *player);
}

void Controller::OnMessage(const Input::MessagePtr& pMsg, Player& player)
{
	bool bOK = pMsg->Process(*this, player);
	ASSERT(bOK);

	SendQueuedMessages();
}

void Controller::SendMessage(const Output::Message& msg, const Player& player) const
{
	SendMessage(std::make_shared<std::string>(msg.GetXML()), player);
}

void Controller::SendMessage(StringPtr msg, const Player& player) const
{
	m_messages[&player].push_back(msg);
}

void Controller::SendMessage(const Output::Message& msg, const Game& game, const Player* pPlayer) const
{
	StringPtr str = std::make_shared<std::string>(msg.GetXML());
	if (!pPlayer)
	{
		for (auto& player : game.GetCurrentPlayers())
			SendMessage(str, *player);
	}
	else
	{
		VERIFY_MODEL_MSG(pPlayer->GetName(), pPlayer->GetCurrentGame() == &game);
		SendMessage(str, *pPlayer);
	}
}

void Controller::SendQueuedMessages()
{
	for (auto& playerMsgs : m_messages)
		for (auto& msg : playerMsgs.second)
			m_pServer->SendMessage(*msg, *playerMsgs.first);
	m_messages.clear();
}

void Controller::ClearQueuedMessages()
{
	m_messages.clear();
}

void Controller::OnPlayerConnected(Player& player)
{
	if (const Game* pGame = player.GetCurrentGame())
		SendUpdateGame(*pGame, &player);
	else
	{
		SendMessage(Output::ShowGameList(), player);
		SendUpdateGameList(&player);
	}

	SendQueuedMessages();
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
			SendMessage(Output::UpdateLobbyControls(*pPlayer), *pPlayer);
		}
		return;
	}

	if (!pLive || pLive->GetGamePhase() == LiveGame::GamePhase::Main)
	{
		SendMessage(Output::ShowGame(), game, pPlayer);
		SendMessage(Output::UpdateTeams(game), game, pPlayer);

		// Send info about each team.
		for (auto& pInfoTeam : game.GetTeams())
		{
			const Player& infoPlayer = pInfoTeam->GetPlayer();
			VERIFY_MODEL_MSG("Team not chosen yet", !!pInfoTeam);
			SendMessage(Output::UpdateTeam(*pInfoTeam), game, pPlayer);
			SendMessage(Output::UpdateInfluenceTrack(*pInfoTeam), game, pPlayer);
			SendMessage(Output::UpdateTechnologyTrack(*pInfoTeam), game, pPlayer);
			SendMessage(Output::UpdateStorageTrack(*pInfoTeam), game, pPlayer);
			SendMessage(Output::UpdatePopulationTrack(*pInfoTeam), game, pPlayer);
			SendMessage(Output::UpdateActionTrack(*pInfoTeam), game, pPlayer);
			SendMessage(Output::UpdateColonyShips(*pInfoTeam), game, pPlayer);
			SendMessage(Output::UpdatePassed(*pInfoTeam), game, pPlayer);
			SendMessage(Output::UpdateBlueprints(*pInfoTeam), game, pPlayer);
			SendMessage(Output::UpdateVictoryPointTiles(*pInfoTeam), game, pPlayer);
			
			// Reputation tile values are secret, so only send them to the relevant player. 
			if (pPlayer)
				SendMessage(Output::UpdateReputationTrack(*pInfoTeam, pPlayer == &infoPlayer), game, pPlayer);
			else
				for (auto& dstPlayer : game.GetCurrentPlayers())
					SendMessage(Output::UpdateReputationTrack(*pInfoTeam, dstPlayer == &infoPlayer), game, dstPlayer);
		}
		SendMessage(Output::UpdateMap(game), game, pPlayer);
		SendMessage(Output::UpdateTechnologies(game), game, pPlayer);
		SendMessage(Output::UpdateRound(game), game, pPlayer);
		SendMessage(Output::AddLog(game.GetLogs()), game, pPlayer);
		SendMessage(Output::UpdateCurrentPlayers(*pLive), *pLive, pPlayer);
	}

	bool isBattle = game.HasBattle();
	SendMessage(Output::UpdateShowCombat(game, isBattle), game, pPlayer);
	if (isBattle)
		SendMessage(Output::UpdateCombat(game, game.GetBattle()), game, pPlayer);

	if (game.HasFinished())
		SendMessage(Output::UpdateScore(game, true), game, pPlayer);

	if (pLive)
		pLive->GetPhase().UpdateClient(*this, pPlayer);
	else if (auto pReview = dynamic_cast<const ReviewGame*>(&game))
		SendMessage(Output::UpdateReviewUI(*pReview), game, pPlayer); // After AddLog.
	else
		ASSERT(false);
}
