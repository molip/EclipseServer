#pragma once

#include "MapPos.h"
#include "Hex.h"

#include "libKernel/Json.h"

#include <string>
#include <memory>
#include <vector>
#include <set>

class Model;
class Game;
class LiveGame;
class ReviewGame;
class Team;
class Player;
class Hex;
class Battle;
class Dice;

enum class ShipPart;

namespace Output
{

class Message
{
public:
	virtual ~Message() {}
	std::string GetXML() const;

protected:
	Json::Document m_doc;
	Json::Element m_root;
};

struct Response : Message
{
	Response();
};

struct Command : Message
{
	Command(const std::string& cmd);
	//virtual void Create() { m_doc.In}
	
	Json::Element* m_pRoot;
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
	Choose(const std::string& param);
};

struct UpdateGameList : Update { UpdateGameList(const Player& player); };
struct UpdateLobby : Update { UpdateLobby(const Game& game); };
struct UpdateLobbyControls : Update { UpdateLobbyControls(const Player& player); };
struct UpdateChoose : Update { UpdateChoose(const LiveGame& game); };
struct UpdateTeams : Update { UpdateTeams(const Game& game); };
struct UpdateTeam : Update { UpdateTeam(const Team& team); };
struct UpdateInfluenceTrack : Update { UpdateInfluenceTrack(const Team& team); };
struct UpdateActionTrack : Update { UpdateActionTrack(const Team& team); };
struct UpdateStorageTrack : Update { UpdateStorageTrack(const Team& team); };
struct UpdateTechnologyTrack : Update { UpdateTechnologyTrack(const Team& team); };
struct UpdatePopulationTrack : Update { UpdatePopulationTrack(const Team& team); };
struct UpdateReputationTrack : Update { UpdateReputationTrack(const Team& team, bool bSendValues); };
struct UpdateVictoryPointTiles : Update { UpdateVictoryPointTiles(const Team& team); };
struct UpdateColonyShips : Update { UpdateColonyShips(const Team& team); };
struct UpdatePassed : Update { UpdatePassed(const Team& team); };
struct UpdateBlueprints : Update { UpdateBlueprints(const Team& team); };
struct UpdateMap : Update { UpdateMap(const Game& game); };
struct UpdateReviewUI : Update { UpdateReviewUI(const ReviewGame& game); };
struct UpdateTechnologies : Update { UpdateTechnologies(const Game& game); };
struct UpdateRound : Update { UpdateRound(const Game& game); };
struct UpdateShowCombat : Update { UpdateShowCombat(const Game& game, bool show); };
struct UpdateCombat : Update { UpdateCombat(const Game& game, const Battle& battle); };
struct UpdateScore : Update { UpdateScore(const Game& game, bool show); };
struct UpdateCurrentPlayers : Update { UpdateCurrentPlayers(const LiveGame& game); };

struct AddLog : Update 
{
	typedef std::vector<std::pair<int, std::string>> Vec;
	AddLog(int id, const std::string& msg);
	AddLog(const Vec& msgs);
};
struct RemoveLog : Update { RemoveLog(int id); };
//struct UpdateUndo : Update { UpdateUndo(bool bEnable); };

struct ChooseTeam : Choose { ChooseTeam(const Game& game, bool bActive); };
struct ChooseAction : Choose { ChooseAction(const LiveGame& game); };
struct ChooseFinished : Choose { ChooseFinished(); };

struct ChoosePositions : Choose { ChoosePositions(const std::vector<MapPos>& positions, const std::string& param); };

struct ChooseExplorePos : ChoosePositions { ChooseExplorePos(const std::vector<MapPos>& positions, bool bCanSkip); };
struct ChooseExploreHex : Choose 
{
	ChooseExploreHex(int x, int y, bool bCanTake, bool bCanUndo);
	void AddHexChoice(int idHex, const std::vector<int>& rotations, bool bCanInfluence);
private:
	Json::Array m_hexes;
};
struct ChooseDiscovery : Choose { ChooseDiscovery(DiscoveryType discovery, bool canKeep, bool canUse); };
struct ChooseColonisePos : Choose { ChooseColonisePos(const std::vector<MapPos>& hexes); };
struct ChooseColoniseSquares : Choose { ChooseColoniseSquares(const MapPos& pos, const SquareCounts& squares, const Population& pop, int nShips); };

struct ChooseInfluencePos : ChoosePositions { ChooseInfluencePos(const std::vector<MapPos>& positions, bool bEnableTrack, const std::string& param); };
struct ChooseInfluenceSrc : ChooseInfluencePos { ChooseInfluenceSrc(const std::vector<MapPos>& positions, bool bEnableTrack, bool bCanAbort, int flips); };
struct ChooseInfluenceDst : ChooseInfluencePos { ChooseInfluenceDst(const std::vector<MapPos>& positions, bool bEnableTrack); };

struct ChooseResearch : Choose { ChooseResearch(const std::vector<std::pair<TechType, int>>& techs, bool bCanSkip); };
struct ChooseResearchArtifact : Choose { ChooseResearchArtifact(int nArtifacts); };

struct ChooseBuild : Choose { ChooseBuild(const std::set<ShipType>& ships, const std::map<MapPos, std::pair<bool, bool>>& hexes, bool bCanSkip); }; // (orbital, monolith)
struct ChooseDiplomacy : Choose { ChooseDiplomacy(); };
struct ChooseMoveSrc : Choose { ChooseMoveSrc(std::map<MapPos, std::set<ShipType>>& srcs, bool bCanSkip); };
struct ChooseMoveDst : Choose { ChooseMoveDst(const std::set<MapPos>& dsts); };
struct ChooseUpgrade : Choose { ChooseUpgrade(const Team& team, std::vector<ShipPart> parts, int allowedUpgrades, bool canRemove); };
struct ChooseTrade : Choose { ChooseTrade(const Team& team); };

struct ChooseUpkeep : Choose { ChooseUpkeep(const Team& team, bool canUndo); };

struct ChooseCombat : Choose { ChooseCombat(const LiveGame& game); };
struct ChooseDice : Choose { ChooseDice(const LiveGame& game, const Dice& dice, int activePlayerId); };

struct ChooseUncolonise : Choose { ChooseUncolonise(const SquareCounts& squares, const Population& pop); };

struct ChooseAutoInfluence : ChoosePositions { ChooseAutoInfluence(const std::vector<MapPos>& positions); };

struct ShowGameList : Show { ShowGameList(); };
struct ShowLobby : Show { ShowLobby(); };
struct ShowChoose : Show { ShowChoose(); };
struct ShowGame : Show { ShowGame(); };
struct ShowScore : Show { ShowScore(const Game& game); };
}
