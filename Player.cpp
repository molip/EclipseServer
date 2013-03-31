#include "stdafx.h"
#include "Player.h"

Player::Player() : m_id(0)
{
}

Player::Player(int id, const std::string& name, const std::string& password) : m_id(id), m_name(name), m_password(password)
{
}

void Player::SetCurrentGame(Game* pGame)
{
	m_pCurrentGame = pGame;
}

