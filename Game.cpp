#include "stdafx.h"
#include "Game.h"
#include "App.h"
#include "Player.h"
#include "CmdStack.h"
#include "EnumTraits.h"
#include "Players.h"
#include "Record.h"
#include "Serial.h"

#include <algorithm>

namespace
{
	const int FakePlayers = 3;
}

Game::Game() : 
	m_id(0), m_idOwner(0), m_iTurn(-1), m_iRound(-1), m_iStartTeam(-1), m_iStartTeamNext(-1), m_map(*this)
{
}

Game::Game(int id, const std::string& name, const Player& owner) : 
	m_id(id), m_name(name), m_idOwner(owner.GetID()), m_iTurn(-1), m_iRound(-1), m_iStartTeam(-1), m_iStartTeamNext(-1),
	m_map(*this)
{
}

Game::Game(int id, const std::string& name, const Player& owner, const Game& rhs) :
	m_id(id), m_name(name), m_idOwner(owner.GetID()), 
	m_iTurn(rhs.m_iTurn), m_iRound(rhs.m_iRound), m_iStartTeam(rhs.m_iRound), m_iStartTeamNext(rhs.m_iStartTeamNext),
	m_map(rhs.m_map, *this), m_techs(rhs.m_techs), m_repBag(rhs.m_repBag), m_techBag(rhs.m_techBag), m_discBag(rhs.m_discBag)
{
	for (auto& t : rhs.m_teams)
		m_teams.push_back(TeamPtr(new Team(*t, id)));

	for (int i = 0; i < (int)HexRing::_Count; ++i)
		m_hexBag[i] = rhs.m_hexBag[i];
}

Game::~Game()
{
}

const Player& Game::GetOwner() const
{
	return Players::Get(m_idOwner);
}

void Game::StartRound()
{
	assert(m_iRound < 9);

	++m_iRound;

	if (m_iRound == 9) // Game over.
		return;

	// Take new technologies from bag.
	const int startTech[] = { 12, 12, 14, 16, 18, 20 };
	const int roundTech[] = { 4, 4, 6, 7, 8, 9 };

	int nTech = (m_iRound == 0 ? startTech : roundTech)[m_teams.size() - 1];
	for (int i = 0; i < nTech && !m_techBag.IsEmpty(); ++i)
		m_techs.insert(m_techBag.TakeTile());
}

Team* Game::FindTeam(const Player& player)
{
	for (auto& t : m_teams)
		if (t->GetPlayerID() == player.GetID())
			return t.get();
	return nullptr;
}

Team& Game::GetTeam(const Player& player)
{
	Team* pTeam = FindTeam(player);
	AssertThrow("Game::GetTeam: player not in game: " + player.GetName(), !!pTeam);
	return *pTeam;
}

const Player& Game::GetCurrentPlayer() const
{
	return Players::Get(GetCurrentTeam().GetPlayerID());
}

Player& Game::GetCurrentPlayer() 
{
	return Players::Get(GetCurrentTeam().GetPlayerID());
}

Team& Game::GetCurrentTeam()
{
	return *m_teams[(m_iStartTeam + m_iTurn) % m_teams.size()];
}

Team* Game::FindTeam(Colour c) 
{
	for (auto& t : m_teams)
		if (t->GetColour() == c)
			return t.get();
	return nullptr;
}

Team& Game::GetTeam(Colour c) 
{
	Team* pTeam = FindTeam(c);
	AssertThrow("Game::GetTeam: colour not in game: ", !!pTeam);
	return *pTeam;
}

void Game::AdvanceTurn()
{
	m_iTurn = (m_iTurn + 1) % m_teams.size();
	Save();
}

void Game::FinishTurn()
{
	AdvanceTurn();
}

void Game::Save(Serial::SaveNode& node) const 
{
	node.SaveType("id", m_id);
	node.SaveType("name", m_name);
	node.SaveType("owner", m_idOwner);
	node.SaveType("turn", m_iTurn);
	node.SaveType("round", m_iRound);
	node.SaveType("start_team", m_iStartTeam);
	node.SaveType("start_team_next", m_iStartTeamNext);
	node.SaveCntr("techs", m_techs, Serial::EnumSaver());
	node.SaveCntr("teams", m_teams, Serial::ClassPtrSaver());
	node.SaveClass("rep_bag", m_repBag);
	node.SaveClass("tech_bag", m_techBag);
	node.SaveClass("disc_bag", m_discBag);
	node.SaveArray("hex_bags", m_hexBag, Serial::ClassSaver());
	node.SaveClass("map", m_map);
}

void Game::Load(const Serial::LoadNode& node)
{
	node.LoadType("id", m_id);
	node.LoadType("name", m_name);
	node.LoadType("owner", m_idOwner);
	node.LoadType("turn", m_iTurn);
	node.LoadType("round", m_iRound);
	node.LoadType("start_team", m_iStartTeam);
	node.LoadType("start_team_next", m_iStartTeamNext);
	node.LoadCntr("techs", m_techs, Serial::EnumLoader());
	node.LoadCntr("teams", m_teams, Serial::ClassPtrLoader());
	node.LoadClass("rep_bag", m_repBag);
	node.LoadClass("tech_bag", m_techBag);
	node.LoadClass("disc_bag", m_discBag);
	node.LoadArray("hex_bags", m_hexBag, Serial::ClassLoader());
	node.LoadClass("map", m_map);

	for (auto& t : m_teams)
		t->SetGameID(m_id);
}
