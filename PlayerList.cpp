#include "stdafx.h"
#include "PlayerList.h"
#include "Players.h"

#pragma warning(push)
#pragma warning(disable : 4244)
#include "PicoSHA2/picosha2.h"
#pragma warning(pop)

namespace
{
	std::string GetHash(const std::string& str)
	{
		std::string hash;
		picosha2::hash256_hex_string(str, hash);
		return hash;
	}
}

PlayerList::PlayerList(const IServer::Request& request) : PlayerList(request.GetQueries().GetInt("player"), request.GetCookies())
{
}

PlayerList::PlayerList(int playerId, const IServer::StringMap& cookies) : m_current(nullptr), m_valid(true), m_explicitPlayer(false)
{
	m_playerCount = cookies.GetInt("player_count");

	for (int i = 0; i < m_playerCount; ++i)
	{
		int id = cookies.GetInt(std::string("player_") + ::FormatInt(i));
		std::string session = cookies.Get(std::string("session_") + ::FormatInt(i));

		if (Player* player = Players::Find(id))
			if (GetHash(session) == player->GetSessionHash())
			{
				m_players.push_back(std::make_pair(player, session));
				if (playerId == player->GetID())
				{
					m_current = player;
					m_explicitPlayer = true;
				}
			}
	}
}

std::string PlayerList::GetResponse(std::string url) const
{
	IServer::Cookies cookies;
	if (!m_valid)
	{
		cookies.Set("player_count", ::FormatInt((int)m_players.size()), true);

		for (int i = 0; i < (int)m_players.size(); ++i)
		{
			cookies.Set(std::string("player_") + ::FormatInt(i), ::FormatInt(m_players[i].first->GetID()), true);
			cookies.Set(std::string("session_") + ::FormatInt(i), m_players[i].second, true);
		}

		for (int i = (int)m_players.size(); i < m_playerCount; ++i)
		{
			cookies.Delete(std::string("player_") + ::FormatInt(i));
			cookies.Delete(std::string("session_") + ::FormatInt(i));
		}
	}

	if (url.empty())
	{
		url = "/";
		if (!m_players.empty())
			url += "?player=" + ::FormatInt((m_current ? m_current : m_players.front().first)->GetID());
	}
	return MongooseServer::CreateRedirectResponse(url, cookies);
}

PlayerList::PlayerVec::const_iterator PlayerList::Find(const Player& player) const
{
	return std::find_if(m_players.begin(), m_players.end(), [&](auto& pair) { return pair.first == &player; });
}

bool PlayerList::Contains(const Player& player) const 
{
	return Find(player) != m_players.end();
}

std::vector<const Player*> PlayerList::GetPlayers() const
{
	std::vector<const Player*> players;
	players.reserve(m_players.size());
	for (auto& pair : m_players)
		players.push_back(pair.first);
	return players;
}

void PlayerList::Add(Player& player)
{
	if (!Contains(player))
	{
		std::ostringstream ss;
		for (int i = 0; i < 4; ++i)
			ss << ::GetRandom()();

		std::string sessionId = ss.str();
		player.SetSessionHash(GetHash(sessionId));
		m_players.insert(m_players.begin(), std::make_pair(&player, sessionId));
	}
	m_current = &player;
	m_valid = false;
}

void PlayerList::RemoveCurrent()
{
	if (!m_current)
		return;

	m_players.erase(Find(*m_current));
	m_current = nullptr;
	m_valid = false;
}
