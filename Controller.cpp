#include "Controller.h"
#include "Model.h"
#include "WSServer.h"
#include "Output.h"
#include "Input.h"

#include "App.h"

SINGLETON(Controller)

Controller::Controller(Model& model) : m_model(model)
{
	model.SetController(this);
}

void Controller::SendUpdateGameList(const std::string& player) const
{
	Output::UpdateGameList msg(m_model);
	if (player.empty())
		m_pServer->BroadcastMessage(msg);
	else
		m_pServer->SendMessage(msg, player);
}

void Controller::OnMessage(const Input::MessagePtr& pMsg, const std::string& player)
{
	bool bOK = pMsg->Process(*this, player);
	ASSERT(bOK);
}

bool Controller::SendMessage(const Output::Message& msg, const std::string& player) const
{
	return m_pServer->SendMessage(msg, player);
}

bool Controller::SendMessage(const Output::Message& msg, const Game& game, const std::string& player) const
{
	std::string str = msg.GetXML();
	if (player.empty())
	{
		auto i = m_games.find(&game);
		AssertThrow("Controller::SendMessage: Game not found", i != m_games.end());
		for (auto& player2 : i->second)
			m_pServer->SendMessage(str, player2);
	}
	else
	{
		AssertThrow("Controller::SendMessage: Player not in game: " + player, &game == GetPlayerGame(player));
		m_pServer->SendMessage(str, player);
	}
	return true;
}

void Controller::OnPlayerConnected(const std::string& player)
{
	bool bOK = m_players.insert(std::make_pair(player, nullptr)).second;
	AssertThrow("Player already connected", bOK);

	m_pServer->SendMessage(Output::ShowGameList(), player);
	SendUpdateGameList(player);
}

void Controller::OnPlayerDisconnected(const std::string& player)
{
	auto i = m_players.find(player);
	AssertThrow("Disconnecting player wasn't connected", i != m_players.end());

	if (const Game* pGame = i->second)
		UnregisterPlayerFromGame(player, pGame);

	m_players.erase(i);
}

void Controller::UnregisterPlayerFromGame(const std::string& player, const Game* pGame)
{
	auto j = m_games.find(pGame);
	AssertThrow("Unregistering player from game: game not found: " + pGame->GetName(), j != m_games.end());

	auto& players = j->second;
	bool bOK = players.erase(player) == 1;
	AssertThrow("Unregistering player from game: player not found in game: " +  pGame->GetName(), bOK);

	if (players.empty())
		m_games.erase(j); // Doesn't matter.

	// TODO: Notify other players in current game. 
}

const Game* Controller::GetPlayerGame(const std::string& player) const 
{
	auto i = m_players.find(player);
	AssertThrow("Controller::GetPlayerGame: Player not found", i != m_players.end());
	return i->second;
}

Game* Controller::GetPlayerGame(const std::string& player) 
{
	return const_cast<Game*>(const_cast<const Controller*>(this)->GetPlayerGame(player));
}

void Controller::SetPlayerGame(const std::string& player, const Game* pGame) 
{
	auto i = m_players.find(player);
	AssertThrow("SetPlayerGame: Player not found", i != m_players.end());

	if (const Game* pOldGame = i->second)
		UnregisterPlayerFromGame(player, pOldGame);

	i->second = pGame;

	if (pGame)
	{
		auto& players = m_games[pGame];
		bool bOK = players.insert(player).second;
		AssertThrow("SetPlayerGame: Player already registered in game: " + pGame->GetName(), bOK);
	}
}

void Controller::SendUpdateGame(const Game& game, const std::string& player) const
{
	if (game.GetPhase() == Game::Phase::ChooseTeam)
	{
		SendMessage(Output::ShowChoose(), game, player);
		SendMessage(Output::UpdateChoose(game), game, player);

		if (player.empty() || player == game.GetCurrentTeamName())
			SendMessage(Output::ActionChoose(game, true), game.GetCurrentTeamName());
	}
	else if(game.GetPhase() == Game::Phase::Main)
	{
		SendMessage(Output::ShowGame(), game, player);
		SendMessage(Output::UpdateTeams(game), game, player);
	
		for (auto& team : game.GetTeams())
		{
			const Team* pTeam = team.second.get();
			AssertThrow("Controller::SendUpdateGame: Team not chosen yet: " + team.first, !!pTeam);
			SendMessage(Output::UpdateTeam(*pTeam), game, player);
		}
		SendMessage(Output::UpdateMap(game), game, player);
	}
}
