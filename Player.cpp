#include "stdafx.h"
#include "Player.h"
#include "Game.h"
#include "Games.h"

Player::Player() : m_id(0), m_idCurrentGame(0)
{
}

Player::Player(int id, const std::string& name, const std::string& password) : 
	m_id(id), m_name(name), m_password(password), m_idCurrentGame(0)
{
}

void Player::SetCurrentGame(Game* pGame)
{
	m_idCurrentGame = pGame ? pGame->GetID() : 0;
}

Game* Player::GetCurrentGame() const
{
	return m_idCurrentGame ? &Games::Get(m_idCurrentGame) : nullptr;
}

LiveGame* Player::GetCurrentLiveGame() const
{
	return m_idCurrentGame ? &Games::GetLive(m_idCurrentGame) : nullptr;
}

Team* Player::GetCurrentTeam()
{
	return m_idCurrentGame ? &GetCurrentGame()->GetTeam(*this) : nullptr;
}
