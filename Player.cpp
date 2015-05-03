#include "stdafx.h"
#include "Player.h"
#include "Game.h"
#include "ReviewGame.h"
#include "Games.h"
#include "MongooseServer.h"
#include "Players.h"

#include "bcrypt/bcrypt.h"

Player::Player() : m_id(0), m_idCurrentGame(0)
{
}

Player::Player(int id, const std::string& email, const std::string& name, const std::string& password) :
	m_id(id), m_email(email), m_name(name), m_idCurrentGame(0)
{
	m_passwordHash = HashPassword(password);

	Save();
}

bool Player::CheckPassword(const std::string& password) const
{
	if (password.empty())
		return false;

	char hash[BCRYPT_HASHSIZE];
	VERIFY(::bcrypt_hashpw(password.c_str(), m_passwordHash.c_str(), hash) == 0);
	
	return m_passwordHash == hash;
}

std::string Player::HashPassword(const std::string& password) const
{
	char salt[BCRYPT_HASHSIZE];
	VERIFY(::bcrypt_gensalt(12, salt) == 0);

	char hash[BCRYPT_HASHSIZE];
	VERIFY(::bcrypt_hashpw(password.c_str(), salt, hash) == 0);

	return hash;
}

void Player::SetSessionHash(const std::string& hash)  
{
	m_sessionHash = hash; 
	Save(); 
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

	Save();
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

void Player::Save() const
{
	if (m_id < 0)
		return;

	std::string path = ::FormatString("%0%1.xml", Players::GetPath(), m_id);
	VERIFY_SERIAL_MSG(path, Serial::SaveClass(path, *this));
}

void Player::Save(Serial::SaveNode& node) const
{
	int idGame = Games::IsReviewGame(m_idCurrentGame) ? GetCurrentReviewGame()->GetLiveGameID() : m_idCurrentGame;

	node.SaveType("id", m_id);
	node.SaveType("current_game", idGame);
	node.SaveType("email", m_email);
	node.SaveType("name", m_name);
	node.SaveType("hash", m_passwordHash);
	node.SaveType("session_hash", m_sessionHash);
}

void Player::Load(const Serial::LoadNode& node)
{
	node.LoadType("id", m_id);
	node.LoadType("current_game", m_idCurrentGame);
	node.LoadType("email", m_email);
	node.LoadType("name", m_name);
	node.LoadType("hash", m_passwordHash);
	node.LoadType("session_hash", m_sessionHash);
}

void Player::RejoinCurrentGame()
{
	if (Games::IsGame(m_idCurrentGame))
		GetCurrentGame()->AddPlayer(this);
	else
		m_idCurrentGame = 0;
}
