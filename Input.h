#pragma once

#include <string>
#include <memory>
#include "tinyxml.h"

typedef TiXmlDocument XmlDoc;

class Controller;
class Player;

namespace Input 
{

class Message
{
public:
	virtual ~Message() {}
	virtual bool Process(Controller& controller, Player& player) const { return true; }
};

typedef std::unique_ptr<Message> MessagePtr;
MessagePtr CreateMessage(const std::string& xml);

//-----------------------------------------------------------------------------

struct Register: Message 
{
	Register(const TiXmlElement& node);
	int GetPlayerID() const { return m_idPlayer; }
private:
	int m_idPlayer;
};

struct JoinGame: Message 
{
	JoinGame(const TiXmlElement& node);
	virtual bool Process(Controller& controller, Player& player) const override; 
	int m_idGame;
};

struct ExitGame : Message // Returns player to game list - they're still in the game.
{
	virtual bool Process(Controller& controller, Player& player) const override; 
};

struct CreateGame : Message 
{
	virtual bool Process(Controller& controller, Player& player) const override; 
};

struct StartGame : Message 
{
	virtual bool Process(Controller& controller, Player& player) const override; 
};

struct ChooseTeam : Message 
{
	ChooseTeam(const TiXmlElement& node);
	virtual bool Process(Controller& controller, Player& player) const override; 
	std::string m_race, m_colour;
};

}