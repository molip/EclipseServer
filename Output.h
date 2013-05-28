#pragma once

#include "MapPos.h"
#include "Hex.h"
#include "Xml.h"

#include <string>
#include <memory>
#include <vector>
#include <set>

class Model;
class Game;
class Team;
class Player;
class Hex;

namespace Output
{

class Message
{
public:
	virtual ~Message() {}
	std::string GetXML() const;

protected:
	Xml::Document m_doc;
	Xml::Element m_root;
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

struct Choose : Command
{
	Choose(const std::string& param, bool bActive = true);
};

struct UpdateGameList : Update { UpdateGameList(const Model& model); };
struct UpdateLobby : Update { UpdateLobby(const Game& game); };
struct UpdateLobbyControls : Update { UpdateLobbyControls(bool bShow); };
struct UpdateChoose : Update { UpdateChoose(const Game& game); };
struct UpdateTeams : Update { UpdateTeams(const Game& game); };
struct UpdateTeam : Update { UpdateTeam(const Team& team); };
struct UpdateInfluenceTrack : Update { UpdateInfluenceTrack(const Team& team); };
struct UpdateStorageTrack : Update { UpdateStorageTrack(const Team& team); };
struct UpdateTechnologyTrack : Update { UpdateTechnologyTrack(const Team& team); };
struct UpdatePopulationTrack : Update { UpdatePopulationTrack(const Team& team); };
struct UpdateReputationTrack : Update { UpdateReputationTrack(const Team& team, bool bSendValues); };
struct UpdateMap : Update { UpdateMap(const Game& game); };
//struct UpdateUndo : Update { UpdateUndo(bool bEnable); };

struct ChooseTeam : Choose { ChooseTeam(const Game& game, bool bActive); };
struct ChooseAction : Choose { ChooseAction(const Game& game); };
struct ChooseFinished : Choose { ChooseFinished(); };

struct ChooseExplorePos : Choose { ChooseExplorePos(const std::vector<MapPos>& positions, bool bCanSkip); };
struct ChooseExploreHex : Choose 
{
	ChooseExploreHex(int x, int y, bool bCanTake, bool bCanUndo);
	void AddHexChoice(int idHex, const std::vector<int>& rotations, bool bCanInfluence);
};
struct ChooseDiscovery : Choose { ChooseDiscovery(bool bCanUndo); };
struct ChooseColonisePos : Choose { ChooseColonisePos(const std::vector<MapPos>& hexes); };
struct ChooseColoniseSquares : Choose { ChooseColoniseSquares(const int squares[SquareType::_Count], const Population& pop, int nShips); };

struct ChooseInfluencePos : Choose { ChooseInfluencePos(const std::vector<MapPos>& positions, bool bEnableTrack, const std::string& param); };
struct ChooseInfluenceSrc : ChooseInfluencePos { ChooseInfluenceSrc(const std::vector<MapPos>& positions, bool bEnableTrack); };
struct ChooseInfluenceDst : ChooseInfluencePos { ChooseInfluenceDst(const std::vector<MapPos>& positions, bool bEnableTrack); };

struct ShowGameList : Show { ShowGameList(); };
struct ShowLobby : Show { ShowLobby(); };
struct ShowChoose : Show { ShowChoose(); };
struct ShowGame : Show { ShowGame(); };

}
