#include "Input.h"
#include "App.h"
#include "Model.h"
#include "Controller.h"
#include "Output.h"
#include "Race.h"
#include "Player.h"

#include <sstream>

namespace Input 
{

RaceType GetRaceFromName(const std::string& race)
{
	if (race == "eridani") return RaceType::Eridani;
	if (race == "hydran") return RaceType::Hydran;
	if (race == "planta") return RaceType::Planta;
	if (race == "descendants") return RaceType::Descendants;
	if (race == "mechanema") return RaceType::Mechanema;
	if (race == "orion") return RaceType::Orion;
	if (race == "human") return RaceType::Human;

	return RaceType::None;
}

Colour GetColourFromName(const std::string& colour)
{
	if (colour == "black") return Colour::Black;
	if (colour == "blue") return Colour::Blue;
	if (colour == "green") return Colour::Green;
	if (colour == "red") return Colour::Red;
	if (colour == "white") return Colour::White;
	if (colour == "yellow") return Colour::Yellow;

	return Colour::None;
}

MessagePtr CreateCommand(const std::string& type, const TiXmlElement& root)
{
	if (type == "register")
		return MessagePtr(new Register(root));
	if (type == "join_game")
		return MessagePtr(new JoinGame(root));
	if (type == "exit_game")
		return MessagePtr(new ExitGame);
	if (type == "create_game")
		return MessagePtr(new CreateGame);
	if (type == "start_game")
		return MessagePtr(new StartGame);
	if (type == "choose_team")
		return MessagePtr(new ChooseTeam(root));
	if (type == "choose_action")
		return MessagePtr(new ChooseAction(root));

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
	
Register::Register(const TiXmlElement& node) : m_idPlayer(0)
{
	node.Attribute("player", &m_idPlayer);
	AssertThrowXML("Register", m_idPlayer > 0);
}

JoinGame::JoinGame(const TiXmlElement& node) : m_idGame(0)
{
	node.Attribute("game", &m_idGame);
	AssertThrowXML("JoinGame", m_idGame > 0);
}

namespace 
{
	void DoJoinGame(Controller& controller, Player& player, Game& game)
	{
		AssertThrow("DoJoinGame: Player already in a game: " + player.GetName(), !player.GetCurrentGame());
		player.SetCurrentGame(&game);
		if (!game.HasStarted())
		{
			game.AddTeam(player); // Might already have joined,  doesn't matter.
			controller.SendMessage(Output::UpdateLobby(game), game);
			controller.SendUpdateGameList();
		}
		controller.SendUpdateGame(game, &player);
	}
}

bool JoinGame::Process(Controller& controller, Player& player) const 
{
	Game* pGame = controller.GetModel().FindGame(m_idGame);
	AssertThrow("JoinGame: game does not exist: " + FormatInt(m_idGame), !!pGame);

	DoJoinGame(controller, player, *pGame);
	return true;
}

bool ExitGame::Process(Controller& controller, Player& player) const 
{
	Game* pGame = player.GetCurrentGame();
	AssertThrow("ExitGame: Player not in any game: " + player.GetName(), !!pGame);

	player.SetCurrentGame(nullptr);
	controller.SendMessage(Output::ShowGameList(), player);
	controller.SendUpdateGameList(&player);
	return true;
}

bool CreateGame::Process(Controller& controller, Player& player) const 
{
	Model& model = controller.GetModel();
	
	std::ostringstream ss;
	ss << "Game " <<  model.GetGames().size() + 1;
	Game& game = model.AddGame(ss.str(), player);

	DoJoinGame(controller, player, game);

	return true;	
}

bool StartGame::Process(Controller& controller, Player& player) const 
{
	Model& model = controller.GetModel();
	
	Game* pGame = player.GetCurrentGame();

	AssertThrow("StartGame: player not registered in any game", !!pGame);
	AssertThrow("StartGame: player isn't the owner of game: " + pGame->GetName(), &player == &pGame->GetOwner());
	AssertThrow("StartGame: game already started: " + pGame->GetName(), !pGame->HasStarted());
	
	pGame->StartChooseTeamPhase();

	controller.SendUpdateGameList();
	controller.SendUpdateGame(*pGame);

	return true;	
}

ChooseTeam::ChooseTeam(const TiXmlElement& node)
{
	m_race = node.Attribute("race");
	m_colour = node.Attribute("colour");
}

bool ChooseTeam::Process(Controller& controller, Player& player) const 
{
	Game* pGame = player.GetCurrentGame();
	AssertThrow("ChooseTeam: player not registered in any game", !!pGame);
	AssertThrow("ChooseTeam: player played out of turn", &player == &pGame->GetCurrentPlayer());
	AssertThrow("ChooseTeam: game not in choose phase: " + pGame->GetName(), pGame->GetPhase() == Game::Phase::ChooseTeam);

	RaceType race = GetRaceFromName(m_race);
	Colour colour = GetColourFromName(m_colour);

	AssertThrowXML("ChooseTeam:race", race != RaceType::None);
	AssertThrowXML("ChooseTeam:colour", colour != Colour::None);
	AssertThrowXML("ChooseTeam: colour already taken", !pGame->GetTeamFromColour(colour));

	if (race != RaceType::Human)
		AssertThrowXML("ChooseTeam: colour doesn't match race", colour == Race(race).GetColour());

	pGame->AssignTeam(player, race, colour);

	controller.SendMessage(Output::ActionChooseTeam(*pGame, false), player);
	controller.SendUpdateGame(*pGame);

	return true;	
}

ChooseAction::ChooseAction(const TiXmlElement& node)
{
	m_action = node.Attribute("action");
}

bool ChooseAction::Process(Controller& controller, Player& player) const 
{
	Game* pGame = player.GetCurrentGame();
	AssertThrow("ChooseAction: player not registered in any game", !!pGame);
	AssertThrow("ChooseAction: player played out of turn", &player == &pGame->GetCurrentPlayer());
	AssertThrow("ChooseAction: game not in main phase: " + pGame->GetName(), pGame->GetPhase() == Game::Phase::Main);

	controller.SendMessage(Output::ActionStartTurn(*pGame, false), player);
	pGame->HaveTurn(player);

	controller.SendMessage(Output::ActionStartTurn(*pGame, true), pGame->GetCurrentPlayer());

	return true;	
}

} // namespace