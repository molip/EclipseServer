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
	pMsg->Process(*this, player);
}

void Controller::OnPlayerRegistered(const std::string& player)
{
	m_pServer->SendMessage(Output::ShowGameList(), player);
	UpdateGameList(player);
}