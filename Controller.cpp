#include "Controller.h"
#include "Model.h"
#include "WSServer.h"
#include "Output.h"

#include "App.h"

Controller::Controller(Model& model) : m_model(model)
{
	model.SetController(this);
}

void Controller::UpdateGameList(const std::string& player) const
{
	m_pServer->SendMessage(Output::UpdateGameList(m_model), player);
}

void Controller::OnMessage(const std::string& player, const std::string& msg)
{
	if (msg == "CREATE_GAME")
	{
		std::ostringstream ss;
		ss << "Game " <<  m_model.GetGames().size() + 1;
		m_model.AddGame(ss.str(), player);
		UpdateGameList();
	}
}

void Controller::OnPlayerRegistered(const std::string& player)
{
	m_pServer->SendMessage(Output::ShowGameList(), player);
	UpdateGameList(player);
}