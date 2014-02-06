#include "stdafx.h"
#include "Game.h"
#include "App.h"
#include "Player.h"
#include "CmdStack.h"
#include "EnumTraits.h"
#include "Players.h"
#include "Record.h"
#include "Serial.h"
#include "Output.h"

#include <algorithm>

namespace
{
	const int FakePlayers = 3;
}

Game::Game() : 
	m_id(0), m_idOwner(0), m_map(*this), m_iRound(-1)
{
}

Game::Game(int id, const std::string& name, const Player& owner) : 
	m_id(id), m_name(name), m_idOwner(owner.GetID()), m_map(*this), m_iRound(-1)
{
}

Game::Game(int id, const std::string& name, const Player& owner, const Game& rhs) :
	m_id(id), m_name(name), m_idOwner(owner.GetID()), 
	m_map(rhs.m_map, *this), m_techs(rhs.m_techs), m_repBag(rhs.m_repBag), m_techBag(rhs.m_techBag), m_discBag(rhs.m_discBag),
	m_iRound(rhs.m_iRound)
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

bool Game::IncrementRound(bool bDo)
{
	m_iRound += bDo ? 1 : -1;

	assert(m_iRound >= 0 && m_iRound <= 9);

	return m_iRound == 9;
}

void Game::Save(Serial::SaveNode& node) const 
{
	node.SaveType("id", m_id);
	node.SaveType("name", m_name);
	node.SaveType("owner", m_idOwner);
	node.SaveCntr("teams", m_teams, Serial::ClassPtrSaver());
	node.SaveClass("map", m_map);
	node.SaveMap("techs", m_techs, Serial::EnumSaver(), Serial::TypeSaver());
	node.SaveClass("rep_bag", m_repBag);
	node.SaveClass("tech_bag", m_techBag);
	node.SaveClass("disc_bag", m_discBag);
	node.SaveArray("hex_bags", m_hexBag, Serial::ClassSaver());
	node.SaveType("round", m_iRound);
}

void Game::Load(const Serial::LoadNode& node)
{
	node.LoadType("id", m_id);
	node.LoadType("name", m_name);
	node.LoadType("owner", m_idOwner);
	node.LoadCntr("teams", m_teams, Serial::ClassPtrLoader());
	node.LoadClass("map", m_map);
	node.LoadMap("techs", m_techs, Serial::EnumLoader(), Serial::TypeLoader());
	node.LoadClass("rep_bag", m_repBag);
	node.LoadClass("tech_bag", m_techBag);
	node.LoadClass("disc_bag", m_discBag);
	node.LoadArray("hex_bags", m_hexBag, Serial::ClassLoader());
	node.LoadType("round", m_iRound);

	for (auto& t : m_teams)
		t->SetGameID(m_id);
}

DEFINE_ENUM_NAMES(HexRing) { "Inner", "Middle", "Outer", "" };
