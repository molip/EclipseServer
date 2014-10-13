#include "stdafx.h"
#include "Player.h"
#include "Game.h"
#include "Games.h"
#include "MongooseServer.h"

Player::Player() : m_id(0), m_idCurrentGame(0)
{
}

Player::Player(int id, const std::string& name, const std::string& password) : 
	m_id(id), m_name(name), m_idCurrentGame(0)
{
	m_passwordSalt = ::FormatInt(::GetRandom()());
	m_passwordHash = HashPassword(password);
}

bool Player::CheckPassword(const std::string& password) const
{
	std::string hash = HashPassword(password);
	return hash == m_passwordHash;
}

std::string Player::HashPassword(const std::string& password) const
{
	return MongooseServer::CreateMD5(password.c_str(), m_passwordSalt.c_str());
}

void Player::SetCurrentGame(const Game* pGame)
{
	const Game* oldGame = GetCurrentGame();

	if (oldGame == pGame)
		return;

	if (oldGame)
		oldGame->RemovePlayer(this);

	m_idCurrentGame = pGame ? pGame->GetID() : 0;

	if (pGame)
		pGame->AddPlayer(this);
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
	return m_idCurrentGame ? GetCurrentGame()->FindTeam(*this) : nullptr;
}
