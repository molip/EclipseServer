#pragma once

#include "Resources.h"

#include <string>
#include <memory>

class Controller;
class Player;
class Game;
class LiveGame; 

enum class ShipType;
enum class Buildable;
enum class ShipPart;

namespace Json { class Element; class Array; }

namespace Input 
{

class Message
{
public:
	virtual ~Message() {}
	virtual bool Process(Controller& controller, Player& player) const { return true; }

protected:
	const LiveGame& GetLiveGame(const Player& player) const;
};

typedef std::unique_ptr<Message> MessagePtr;
MessagePtr CreateMessage(const std::string& xml);

//-----------------------------------------------------------------------------

struct Register: Message 
{
	Register(const Json::Element& node);
	int GetPlayerID() const { return m_idPlayer; }
private:
	int m_idPlayer;
};

struct JoinGame: Message 
{
	JoinGame(const Json::Element& node);
	virtual bool Process(Controller& controller, Player& player) const override; 
	int m_idGame;
};

struct ExitGame : Message // Returns player to game list - they're still in the game.
{
	virtual bool Process(Controller& controller, Player& player) const override; 
};

struct StartReview : Message 
{
	virtual bool Process(Controller& controller, Player& player) const override; 
};

struct ExitReview : Message 
{
	virtual bool Process(Controller& controller, Player& player) const override; 
};

struct AdvanceReview : Message 
{
	virtual bool Process(Controller& controller, Player& player) const override; 
};

struct RetreatReview : Message 
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
	ChooseTeam(const Json::Element& node);
	virtual bool Process(Controller& controller, Player& player) const override; 
	std::string m_race, m_colour;
};

struct StartAction : Message 
{
	StartAction(const Json::Element& node);
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

struct FinishUpkeep : Message
{
	virtual bool Process(Controller& controller, Player& player) const override;
};

struct Chat : Message
{
	Chat(const Json::Element& node);
	virtual bool Process(Controller& controller, Player& player) const override;
	std::string m_msg;
};

struct SlotChange
{
	ShipType ship;
	int slot;
	ShipPart part;
};
struct SlotChanges : std::vector<SlotChange>
{
	SlotChanges() {}
	SlotChanges(const Json::Array& node);
};
struct QueryBlueprintStats : Message
{
	QueryBlueprintStats(const Json::Element& node);
	virtual bool Process(Controller& controller, Player& player) const override;
	SlotChanges m_changes;
};

//-----------------------------------------------------------------------------

class CmdMessage : public Message
{
public:
	virtual bool Process(Controller& controller, Player& player) const;
};

struct CmdExplorePos : CmdMessage
{
	CmdExplorePos(const Json::Element& node);
	int m_iPos;
};

struct CmdExploreHex : CmdMessage
{
	CmdExploreHex(const Json::Element& node);
	int m_iRot;
	int m_iHex;
	bool m_bInfluence;
};

struct CmdExploreHexTake : CmdMessage {};

struct CmdDiscovery : CmdMessage
{
	CmdDiscovery(const Json::Element& node);
	enum class Action { None, Points, Use, Keep };
	Action m_action;
};

struct CmdColonisePos : CmdMessage
{
	CmdColonisePos(const Json::Element& node);
	int m_iPos;
};

struct CmdColoniseBase : CmdMessage
{
	CmdColoniseBase(const Json::Element& node);
	
	Population m_moved;
};

struct CmdColoniseSquares : CmdColoniseBase
{
	CmdColoniseSquares(const Json::Element& node);
};
struct CmdUncolonise : CmdColoniseBase
{
	CmdUncolonise(const Json::Element& node);
};

struct CmdAbort : CmdMessage
{
};

struct CmdInfluenceSrc : CmdMessage
{
	CmdInfluenceSrc(const Json::Element& node);
	int m_iPos;
};

struct CmdInfluenceFlip : CmdMessage
{
	CmdInfluenceFlip(const Json::Element& node);
};

struct CmdInfluenceDst : CmdMessage
{
	CmdInfluenceDst(const Json::Element& node);
	int m_iPos;
};

struct CmdResearch : CmdMessage
{
	CmdResearch(const Json::Element& node);
	int m_iTech;
};

struct CmdResearchArtifact : CmdMessage
{
	CmdResearchArtifact(const Json::Element& node);
	Storage m_artifacts;
};

struct CmdMoveSrc : CmdMessage
{
	CmdMoveSrc(const Json::Element& node);
	int m_x, m_y;
	ShipType m_ship;
};

struct CmdMoveDst : CmdMessage
{
	CmdMoveDst(const Json::Element& node);
	int m_x, m_y;
};

struct CmdBuild : CmdMessage
{
	CmdBuild(const Json::Element& node);
	int m_x, m_y;
	Buildable m_buildable;
};

struct CmdDiplomacy : CmdMessage
{
	CmdDiplomacy(const Json::Element& node);
};

struct CmdUpgrade : CmdMessage
{
	CmdUpgrade(const Json::Element& node);
	SlotChanges m_changes;
};

struct CmdTrade : CmdMessage
{
	CmdTrade(const Json::Element& node);
	Resource m_from, m_to;
	int m_count; // Count of "to" resource produced.
};

struct CmdCombat : CmdMessage
{
	CmdCombat(const Json::Element& node);
	bool m_fire;
};

struct CmdDice : CmdMessage
{
	CmdDice(const Json::Element& node);
};

}