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
	if (type == "start_game")
		return MessagePtr(new StartGame(root));

	AssertThrow("Input command not recognised: " + type);
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

namespace 
{
	// Should be a command?
	void DoJoinGame(Controller& controller, const std::string& player, Game& game)
	{
		controller.SetPlayerGame(player,  &game);

		if (game.HasStarted())
		{
			controller.SendMessage(Output::ShowGame(), player);
			controller.SendMessage(Output::UpdateGame(game), game);
		}
		else
		{
			game.AddPlayer(player); // Might already have joined,  doesn't matter.
			controller.SendMessage(Output::ShowLobby(), player);
			controller.SendMessage(Output::UpdateLobby(game), game);
		}
		controller.SendUpdateGameList();
	}
}

bool JoinGame::Process(Controller& controller, const std::string& player) const 
{
	Game* pGame = controller.GetModel().FindGame(m_game);
	AssertThrow("JoinGame: game does not exist: " + m_game, !!pGame);

	DoJoinGame(controller, player, *pGame);
	return true;
}

bool CreateGame::Process(Controller& controller, const std::string& player) const 
{
	Model& model = controller.GetModel();
	
	std::ostringstream ss;
	ss << "Game " <<  model.GetGames().size() + 1;
	Game& game = model.AddGame(ss.str(), player);

	DoJoinGame(controller, player, game);

	return true;	
}

StartGame::StartGame(const TiXmlElement& node)
{
	auto p = node.Attribute("game");
	AssertThrowXML("StartGame", !!p);
	m_game = p;
}

bool StartGame::Process(Controller& controller, const std::string& player) const 
{
	Model& model = controller.GetModel();
	
	const Game* pPlayerGame = controller.GetPlayerGame(player);
	Game* pGame = model.FindGame(m_game);

	AssertThrow("StartGame: game not found: " + m_game, !!pGame);
	AssertThrow("StartGame: player isn't the game owner: " + m_game, player == pGame->GetOwner());
	AssertThrow("StartGame: player not registered in game: " + m_game, pPlayerGame == pGame);
	AssertThrow("StartGame: game already started: " + m_game, !pGame->HasStarted());
	
	pGame->Start();

	controller.SendUpdateGameList();
	controller.SendMessage(Output::ShowGame(), *pGame);
	controller.SendMessage(Output::UpdateGame(*pGame), *pGame);

	return true;	
}

} // namespace