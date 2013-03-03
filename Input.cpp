#include "Input.h"
#include "App.h"
#include "Model.h"
#include "Controller.h"
#include "Output.h"

#include <sstream>

namespace Input 
{

Race GetRaceFromName(const std::string& race)
{
	if (race == "eridani") return Race::Eridani;
	if (race == "hydran") return Race::Hydran;
	if (race == "planta") return Race::Planta;
	if (race == "descendants") return Race::Descendants;
	if (race == "mechanema") return Race::Mechanema;
	if (race == "orion") return Race::Orion;
	if (race == "human") return Race::Human;

	return Race::_Count;
}

Colour GetColourFromName(const std::string& colour)
{
	if (colour == "black") return Colour::Black;
	if (colour == "blue") return Colour::Blue;
	if (colour == "green") return Colour::Green;
	if (colour == "red") return Colour::Red;
	if (colour == "white") return Colour::White;
	if (colour == "yellow") return Colour::Yellow;

	return Colour::_Count;
}

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
	if (type == "choose_team")
		return MessagePtr(new ChooseTeam(root));

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
			controller.SendUpdateGame(game, player);
			// TODO: Notify other players.
		}
		else
		{
			game.AddTeam(player); // Might already have joined,  doesn't matter.
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
	controller.SendUpdateGame(*pGame);

	return true;	
}

ChooseTeam::ChooseTeam(const TiXmlElement& node)
{
	m_race = node.Attribute("race");
	m_colour = node.Attribute("colour");
}

bool ChooseTeam::Process(Controller& controller, const std::string& player) const 
{
	Game* pGame = controller.GetPlayerGame(player);
	AssertThrow("ChooseTeam: player not registered in any game", !!pGame);
	AssertThrow("ChooseTeam: player played out of turn", player == pGame->GetCurrentTeamName());
	AssertThrow("ChooseTeam: game not in choose phase: " + pGame->GetName(), pGame->GetPhase() == Game::Phase::ChooseTeam);

	Race race = GetRaceFromName(m_race);
	Colour colour = GetColourFromName(m_colour);

	AssertThrowXML("ChooseTeam:race", race != Race::_Count);
	AssertThrowXML("ChooseTeam:colour", colour != Colour::_Count);

	pGame->AssignTeam(player, race, colour);

	controller.SendMessage(Output::ActionChoose(*pGame, false), player);
	controller.SendUpdateGame(*pGame);

	return true;	
}

} // namespace