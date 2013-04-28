#pragma once

#include <string>
#include <memory>
#include "tinyxml.h"

typedef TiXmlDocument XmlDoc;

class Controller;
class Player;
class Game;

namespace Input 
{

class Message
{
public:
	virtual ~Message() {}
	virtual bool Process(Controller& controller, Player& player) const { return true; }

protected:
	Game& GetGameAssert(Player& player) const;
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

struct StartAction : Message 
{
	StartAction(const TiXmlElement& node);
	virtual bool Process(Controller& controller, Player& player) const override; 
	std::string m_action;
};

struct Undo : Message 
{
	virtual bool Process(Controller& controller, Player& player) const override; 
};

struct Commit : Message 
{
	virtual bool Process(Controller& controller, Player& player) const override; 
};

//-----------------------------------------------------------------------------

class CmdMessage : public Message
{
public:
	virtual bool Process(Controller& controller, Player& player) const;
};

struct CmdExplorePos : CmdMessage
{
	CmdExplorePos(const TiXmlElement& node);
	int m_iPos;
};

struct CmdExploreHex : CmdMessage
{
	CmdExploreHex(const TiXmlElement& node);
	int m_iRot;
	int m_iHex;
	bool m_bInfluence;
};

struct CmdExploreDiscovery : CmdMessage
{
	CmdExploreDiscovery(const TiXmlElement& node);
};

struct CmdExploreReject : CmdMessage
{
};

struct CmdInfluenceSrc : CmdMessage
{
	CmdInfluenceSrc(const TiXmlElement& node);
	int m_iPos;
};

struct CmdInfluenceDst : CmdMessage
{
	CmdInfluenceDst(const TiXmlElement& node);
	int m_iPos;
};

}