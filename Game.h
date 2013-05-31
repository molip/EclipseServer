#pragma once

#include "Map.h"
#include "Team.h"
#include "Bag.h"
#include "Cmd.h"

#include <memory>
#include <set>
#include <map>
#include <string>
#include <deque>
#include <list>

enum class HexRing { Inner, Middle, Outer, _Count };

class CmdStack;
class Record;

class Game
{
public:
	enum class Phase { Lobby, ChooseTeam, Main };

	Game(int id, const std::string& name, Player& owner);
	~Game();
	
	int GetID() const { return m_id; }
	const std::string& GetName() const { return m_name; }
	const Player& GetOwner() const;

	void AddPlayer(Player& player);
	const std::vector<TeamPtr>& GetTeams() const { return m_teams; }

	void AssignTeam(Player& player, RaceType race, Colour colour);
	void HaveTurn(Player& player);
	
	bool HasTeamChosen(const Team& team) const;
	Team* FindTeam(const Player& player);

	const Player& GetCurrentPlayer() const;
	Player& GetCurrentPlayer();

	Team* FindTeam(Colour c);
	Team& GetTeam(Colour c);
	Team& GetTeam(const Player& player);
	Team& GetCurrentTeam();

	const Team* FindTeam(Colour c) const			{ return const_cast<Game*>(this)->FindTeam(c); }
	const Team& GetTeam(Colour c) const				{ return const_cast<Game*>(this)->GetTeam(c); }
	const Team& GetTeam(const Player& player) const	{ return const_cast<Game*>(this)->GetTeam(player); }
	const Team& GetCurrentTeam() const				{ return const_cast<Game*>(this)->GetCurrentTeam(); }

	void StartChooseTeamPhase();
	void StartMainPhase();
	bool HasStarted() const { return m_phase != Phase::Lobby; }
	Phase GetPhase() const { return m_phase; }

	ReputationBag& GetReputationBag() { return m_repBag; }
	TechnologyBag& GetTechnologyBag() { return m_techBag; }
	DiscoveryBag& GetDiscoveryBag() { return m_discBag; }
	HexBag& GetHexBag(HexRing ring) { return m_hexBag[(int)ring]; }

	const Map& GetMap() const { return m_map; }
	Map& GetMap() { return m_map; }

	void StartCmd(CmdPtr pCmd);
	void AddCmd(CmdPtr pCmd);
	Cmd* RemoveCmd(); // Returns cmd to undo.
	bool CanRemoveCmd() const;
	bool CanDoAction() const;

	void FinishTurn();
	Cmd* GetCurrentCmd();
	const Cmd* GetCurrentCmd() const;

	void PushRecord(std::unique_ptr<Record>& pRec);
	std::unique_ptr<Record> PopRecord();

private:
	void StartRound();

	void AssertStarted() const;
	void AdvanceTurn();
	
	int m_id; 
	std::string m_name;
	int m_idOwner;
	Phase m_phase;

	Map	m_map;
	std::multiset<TechType> m_techs;

	std::vector<TeamPtr> m_teams;
	
	ReputationBag m_repBag;
	TechnologyBag m_techBag;
	DiscoveryBag m_discBag;
	HexBag m_hexBag[HexRing::_Count];

	std::set<RaceType, Colour> m_availableRaces;
	int m_iTurn, m_iRound;
	int m_iStartTeam, m_iStartTeamNext;

	CmdStack* m_pCmdStack;
	std::list<std::unique_ptr<Record>> m_records;
};

typedef std::unique_ptr<Game> GamePtr;
