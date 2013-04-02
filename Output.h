#pragma once

#include <string>
#include "tinyxml.h"

#include <memory>

typedef TiXmlDocument XmlDoc;

class Model;
class Game;
class Team;

namespace Output
{

class Message
{
public:
	virtual ~Message() {}
	const std::string& GetXML() const;

protected:
	//virtual void Create() {}
	TiXmlElement* AddElement(const std::string& name, TiXmlNode& parent);
	TiXmlElement* AddElement(const std::string& name) { return AddElement(name, m_doc); }
	XmlDoc m_doc;

private:
	mutable std::unique_ptr<TiXmlPrinter> m_pPrinter;
};

struct Command : Message
{
	Command(const std::string& cmd);
	//virtual void Create() { m_doc.In}
	
	TiXmlElement* m_pRoot;
};

struct Show : Command
{
	Show(const std::string& panel);
};

struct Update : Command
{
	Update(const std::string& param);
};

struct Action : Command
{
	Action(const std::string& param, bool bActive = true);
};

struct UpdateGameList : Update { UpdateGameList(const Model& model); };
struct UpdateLobby : Update { UpdateLobby(const Game& game); };
struct UpdateLobbyControls : Update { UpdateLobbyControls(bool bShow); };
struct UpdateChoose : Update { UpdateChoose(const Game& game); };
struct UpdateTeams : Update { UpdateTeams(const Game& game); };
struct UpdateTeam : Update { UpdateTeam(const Team& team); };
struct UpdateMap : Update { UpdateMap(const Game& game); };

struct ActionChooseTeam : Action { ActionChooseTeam(const Game& game, bool bActive); };
struct ActionChooseAction : Action { ActionChooseAction(const Game& game, bool bActive); };

struct ShowGameList : Show { ShowGameList(); };
struct ShowLobby : Show { ShowLobby(); };
struct ShowChoose : Show { ShowChoose(); };
struct ShowGame : Show { ShowGame(); };

}
