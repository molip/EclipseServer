#include "Controller.h"
#include "Model.h"
#include "WSServer.h"

#include "App.h"

Controller::Controller(Model& model) : m_model(model)
{
	model.SetController(this);
}

void Controller::UpdateGameList(const std::string& player) const
{
	std::string msg = "UPDATE:GAMELIST";
	
	for (auto& g : m_model.GetGames())
	{
		msg += ":" + g->GetName();
	}
	m_pServer->SendMessage(msg, player);
}

void Controller::OnCommand(const std::string& player, const std::string& cmd, const std::vector<std::string>& params)
{
	if (cmd == "CREATE_GAME")
	{
		std::ostringstream ss;
		ss << "Game " <<  m_model.GetGames().size() + 1;
		m_model.AddGame(ss.str(), player);
		UpdateGameList();
	}
	
}
