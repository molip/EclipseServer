#include "PlayerList.h"
#include "Players.h"

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

		if (const Player* player = Players::Find(id))
			if (session == player->GetPasswordHash())
			{
				m_players.push_back(player);
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
			cookies.Set(std::string("player_") + ::FormatInt(i), ::FormatInt(m_players[i]->GetID()), true);
			cookies.Set(std::string("session_") + ::FormatInt(i), m_players[i]->GetPasswordHash(), true);
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
			url += "?player=" + ::FormatInt((m_current ? m_current : m_players.front())->GetID());
	}
	return MongooseServer::CreateRedirectResponse(url, cookies);
}

bool PlayerList::Contains(const Player& player) const 
{
	return std::find(m_players.begin(), m_players.end(), &player) != m_players.end(); 
}

void PlayerList::Add(const Player& player)
{
	if (!Contains(player))
		m_players.insert(m_players.begin(), &player);
	m_current = &player;
	m_valid = false;
}

void PlayerList::RemoveCurrent()
{
	if (!m_current)
		return;

	m_players.erase(std::find(m_players.begin(), m_players.end(), m_current));
	m_current = nullptr;
	m_valid = false;
}
