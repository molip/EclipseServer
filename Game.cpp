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
#include "Battle.h"

#include <algorithm>

Game::Game() : 
	m_id(0), m_idOwner(0), m_state(*this)
{
}

Game::Game(int id, const std::string& name, const Player& owner) : 
	m_id(id), m_name(name), m_idOwner(owner.GetID()), m_state(*this)
{
}

Game::Game(int id, const std::string& name, const Player& owner, const Game& rhs) :
	m_id(id), m_name(name), m_idOwner(owner.GetID()), m_state(rhs.m_state, *this),
	 m_repBag(rhs.m_repBag), m_techBag(rhs.m_techBag), m_discBag(rhs.m_discBag)
{
	for (auto& t : rhs.m_teams)
	{
		auto& teamState = m_state.GetTeamState(t->GetColour());
		m_teams.push_back(TeamPtr(new Team(*t)));
		m_teams.back()->SetState(teamState);
	}

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
	VERIFY_MODEL_MSG(player.GetName(), !!pTeam);
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
	VERIFY_MODEL_MSG("colour not in game: ", !!pTeam);
	return *pTeam;
}

bool Game::IncrementRound(bool bDo)
{
	m_state.m_iRound += bDo ? 1 : -1;

	assert(m_state.m_iRound >= 0 && m_state.m_iRound <= 9);

	return m_state.m_iRound == 9;
}

Battle& Game::GetBattle() 
{
	VERIFY_MODEL(!!m_state.m_battle);
	return *m_state.m_battle;
}

void Game::AttachBattle(BattlePtr battle)
{ 
	m_state.m_battle = std::move(battle);
}

BattlePtr Game::DetachBattle()
{
	return std::move(m_state.m_battle);
}

void Game::Save(Serial::SaveNode& node) const 
{
	node.SaveType("id", m_id);
	node.SaveType("name", m_name);
	node.SaveType("owner", m_idOwner);
	node.SaveCntr("teams", m_teams, Serial::ClassPtrSaver());
	node.SaveClass("rep_bag", m_repBag);
	node.SaveClass("tech_bag", m_techBag);
	node.SaveClass("disc_bag", m_discBag);
	node.SaveArray("hex_bags", m_hexBag, Serial::ClassSaver());

	node.SaveClass("state", m_state);
}

void Game::Load(const Serial::LoadNode& node)
{
	node.LoadType("id", m_id);
	node.LoadType("name", m_name);
	node.LoadType("owner", m_idOwner);
	node.LoadCntr("teams", m_teams, Serial::ClassPtrLoader());
	node.LoadClass("rep_bag", m_repBag);
	node.LoadClass("tech_bag", m_techBag);
	node.LoadClass("disc_bag", m_discBag);
	node.LoadArray("hex_bags", m_hexBag, Serial::ClassLoader());
	
	node.LoadClass("state", m_state);

	for (auto& t : m_teams)
	{
		if (t->IsAssigned())
		{
			t->SetState(m_state.GetTeamState(t->GetColour()));
			t->InitState();
		}
	}
}

DEFINE_ENUM_NAMES2(HexRing, -1) { "None", "Inner", "Middle", "Outer", "" };
