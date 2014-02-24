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

void Player::SetCurrentGame(const Game* pGame)
{
	m_idCurrentGame = pGame ? pGame->GetID() : 0;
}

const Game* Player::GetCurrentGame() const
{
	return m_idCurrentGame ? &Games::Get(m_idCurrentGame) : nullptr;
}

const LiveGame* Player::GetCurrentLiveGame() const
{
	return m_idCurrentGame ? &Games::GetLive(m_idCurrentGame) : nullptr;
}

const ReviewGame* Player::GetCurrentReviewGame() const
{
	return m_idCurrentGame ? &Games::GetReview(m_idCurrentGame) : nullptr;
}

const Team* Player::GetCurrentTeam() const
{
	return m_idCurrentGame ? &GetCurrentGame()->GetTeam(*this) : nullptr;
}
