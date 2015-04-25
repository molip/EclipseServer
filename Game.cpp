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

const int Game::RoundCount = 9;

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
	 m_techBag(rhs.m_techBag), m_discBag(rhs.m_discBag)
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

bool Game::HasFinished() const 
{ 
	return m_state.m_iRound == Game::RoundCount; 
}

Game::Scores Game::GetScores() const
{
	Scores scores;

	for (auto& team : GetTeams())
		scores.insert(std::make_pair(team->GetScore(*this), team.get()));

	return scores;
}

void Game::Save(Serial::SaveNode& node) const 
{
	node.SaveType("id", m_id);
	node.SaveType("name", m_name);
	node.SaveType("owner", m_idOwner);
	node.SaveCntr("teams", m_teams, Serial::ClassPtrSaver());
	node.SaveClass("tech_bag", m_techBag);
	node.SaveClass("disc_bag", m_discBag);
	node.SaveArray("hex_bags", m_hexBag, Serial::ClassSaver());
}

void Game::Load(const Serial::LoadNode& node)
{
	node.LoadType("id", m_id);
	node.LoadType("name", m_name);
	node.LoadType("owner", m_idOwner);
	node.LoadCntr("teams", m_teams, Serial::ClassPtrLoader());
	node.LoadClass("tech_bag", m_techBag);
	node.LoadClass("disc_bag", m_discBag);
	node.LoadArray("hex_bags", m_hexBag, Serial::ClassLoader());
}

DEFINE_ENUM_NAMES2(HexRing, -1) { "None", "Inner", "Middle", "Outer", "" };
