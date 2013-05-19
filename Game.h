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

enum class HexRing { Inner, Middle, Outer, _Count };

class CmdStack;

class Game
{
public:
	enum class Phase { Lobby, ChooseTeam, Main };

	Game(int id, const std::string& name, Player& owner);
	~Game();
	
	int GetID() const { return m_id; }
	const std::string& GetName() const { return m_name; }
	const Player& GetOwner() const { return m_owner; }

	bool AddTeam(Player& player);
	const std::map<Player*, TeamPtr>& GetTeams() const { return m_teams; }
	const std::vector<Player*>& GetTeamOrder() const { return m_teamOrder; }

	void AssignTeam(Player& player, RaceType race, Colour colour);
	void HaveTurn(Player& player);
	
	bool HasTeamChosen(Player& player) const;
	const Team& GetTeam(const Player& player) const;
	Team& GetTeam(Player& player);
	const Player& GetCurrentPlayer() const;
	Player& GetCurrentPlayer();
	const Team& GetCurrentTeam() const;
	Team& GetCurrentTeam();
	const Team* GetTeamFromColour(Colour c) const;

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

private:
	void StartRound();

	void AssertStarted() const;
	void AdvanceTurn();
	
	int m_id; 
	std::string m_name;
	Player& m_owner;
	Phase m_phase;

	Map	m_map;
	std::multiset<TechType> m_techs;

	std::map<Player*, TeamPtr> m_teams;
	
	ReputationBag m_repBag;
	TechnologyBag m_techBag;
	DiscoveryBag m_discBag;
	HexBag m_hexBag[HexRing::_Count];

	std::vector<Player*> m_teamOrder;
	std::set<RaceType, Colour> m_availableRaces;
	int m_iTurn, m_iRound;
	int m_iStartTeam, m_iStartTeamNext;

	CmdStack* m_pCmdStack;
};

typedef std::unique_ptr<Game> GamePtr;
