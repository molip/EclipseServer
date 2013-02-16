#pragma once

#include <string>
#include <memory>
#include "tinyxml.h"

typedef TiXmlDocument XmlDoc;

class Controller;

namespace Input 
{

class Message
{
public:
	virtual ~Message() {}
	virtual bool Process(Controller& controller, const std::string& player) const { return true; }
};

typedef std::unique_ptr<Message> MessagePtr;
MessagePtr CreateMessage(const std::string& xml);

//-----------------------------------------------------------------------------

struct Register: Message 
{
	Register(const TiXmlElement& node);
	const std::string& GetPlayer() const { return m_player; }
private:
	std::string m_player;
};

struct JoinGame: Message 
{
	JoinGame(const TiXmlElement& node);
	virtual bool Process(Controller& controller, const std::string& player) const override; 
	std::string m_game;
};

class CreateGame : public Message 
{
	virtual bool Process(Controller& controller, const std::string& player) const override; 
};

}