#include "Input.h"
#include "App.h"
#include "Model.h"
#include "Controller.h"
#include "Output.h"

#include <sstream>

namespace Input 
{
MessagePtr CreateCommand(const std::string& type, const TiXmlElement& root)
{
	if (type == "register")
		return MessagePtr(new Register(root));
	if (type == "join_game")
		return MessagePtr(new JoinGame(root));
	if (type == "create_game")
		return MessagePtr(new CreateGame);
	return nullptr;
}

MessagePtr CreateMessage(const std::string& xml)
{
	XmlDoc doc;
	doc.Parse(xml.c_str());
	if (doc.Error())
		return false;

	if (TiXmlElement* pRoot = doc.RootElement())
		if (auto pTypeStr = pRoot->Attribute("type"))
			return CreateCommand(pTypeStr, *pRoot);
	
	return nullptr;
}

//-----------------------------------------------------------------------------
	
Register::Register(const TiXmlElement& node)
{
	auto p = node.Attribute("player");
	AssertThrowXML("Register", !!p);
	m_player = p;
}

JoinGame::JoinGame(const TiXmlElement& node)
{
	auto p = node.Attribute("game");
	AssertThrowXML("JoinGame", !!p);
	m_game = p;
}

bool JoinGame::Process(Controller& controller, const std::string& player) const 
{
	Model& model = controller.GetModel();
	
	Player* pPlayer = controller.FindPlayer(player);
	if (!ASSERT(pPlayer))
		return false;

	if (pPlayer->GetCurrentGame())
		return false;

	if (Game* pGame = model.FindGame(m_game))
	{
		pGame->AddPlayer(player); // Might already have joined.
		pPlayer->SetCurrentGame(pGame);

		controller.UpdateGameList();

		if (pGame->HasStarted())
		{
			controller.SendMessage(Output::ShowGame(), player);
			controller.SendMessage(Output::UpdateGame(*pGame));
		}
		else
		{
			controller.SendMessage(Output::ShowLobby(), player);
			controller.SendMessage(Output::UpdateLobby(*pGame));
		}

		return true;	
	}
	return false;
}

bool CreateGame::Process(Controller& controller, const std::string& player) const 
{
	Model& model = controller.GetModel();
	
	std::ostringstream ss;
	ss << "Game " <<  model.GetGames().size() + 1;
	Game& game = model.AddGame(ss.str(), player);
	game.AddPlayer(player); 

	controller.UpdateGameList();
	controller.SendMessage(Output::ShowLobby(), player);
	controller.SendMessage(Output::UpdateLobby(game));

	return true;	
}

} // namespace