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

void Controller::UpdateGameList(const std::string& player) const
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
	std::string str = msg.GetXML();
	for (auto& player : game.GetPlayers())
		if (const Player* pPlayer = FindPlayer(player))
			if (pPlayer->GetCurrentGame() == &game)
				m_pServer->SendMessage(str, player);
	return true;
}

void Controller::OnPlayerConnected(const std::string& player)
{
	m_players.AddPlayer(player);
	m_pServer->SendMessage(Output::ShowGameList(), player);
	UpdateGameList(player);
}

void Controller::OnPlayerDisconnected(const std::string& player)
{
	m_players.DeletePlayer(player);
	// TODO: Notify other players in current game. Remove from lobby. 
}

