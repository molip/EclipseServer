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
	m_pServer->SendMessage(Output::UpdateGameList(m_model), player);
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

bool Controller::SendMessage(const Output::Message& msg, const Game& game) const
{
	auto i = m_games.find(&game);
	if (i != m_games.end())
	{
		std::string str = msg.GetXML();
		for (auto& player : i->second)
			m_pServer->SendMessage(str, player);
	}
	return true;
}

void Controller::OnPlayerConnected(const std::string& player)
{
	bool bOK = m_players.insert(std::make_pair(player, nullptr)).second;
	AssertThrow("Player already connected: " + player, bOK);

	m_pServer->SendMessage(Output::ShowGameList(), player);
	SendUpdateGameList(player);
}

void Controller::OnPlayerDisconnected(const std::string& player)
{
	auto i = m_players.find(player);
	AssertThrow("Disconnecting player wasn't connected: " + player, i != m_players.end());

	if (const Game* pGame = i->second)
		UnregisterPlayerFromGame(player, pGame);

	m_players.erase(i);
}

void Controller::UnregisterPlayerFromGame(const std::string& player, const Game* pGame)
{
	auto j = m_games.find(pGame);
	AssertThrow("Unregistering player from game: game not found: " + player, j != m_games.end());

	auto& players = j->second;
	bool bOK = players.erase(player) == 1;
	AssertThrow("Unregistering player from game: player not found in game: " + player, bOK);

	if (players.empty())
		m_games.erase(j); // Doesn't matter.

	// TODO: Notify other players in current game. 
}

const Game* Controller::GetPlayerGame(const std::string& player) const
{
	auto i = m_players.find(player);
	AssertThrow("Controller::GetPlayerGame: Player not found: " + player, i != m_players.end());
	return i->second;
}

void Controller::SetPlayerGame(const std::string& player, const Game* pGame) 
{
	auto i = m_players.find(player);
	AssertThrow("SetPlayerGame: Player not found: " + player, i != m_players.end());

	if (const Game* pOldGame = i->second)
		UnregisterPlayerFromGame(player, pOldGame);

	i->second = pGame;

	if (pGame)
	{
		auto& players = m_games[pGame];
		bool bOK = players.insert(player).second;
		AssertThrow("SetPlayerGame: Player already registered in game: " + player, bOK);
	}
}
