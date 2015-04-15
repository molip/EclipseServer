#pragma once

#include "MongooseServer.h"

class Player;

class PlayerList
{
public:
	PlayerList(const IServer::Request& request);
	PlayerList(int playerId, const IServer::StringMap& cookies);

	std::string GetResponse(std::string url = std::string()) const;
	
	const Player* GetCurrent() const { return m_current; }
	bool IsExplicitPlayer() const { return m_explicitPlayer; }
	bool Contains(const Player& player) const;
	const std::vector<const Player*> GetPlayers() const { return m_players; }

	void Add(const Player& player);
	void RemoveCurrent();

private:
	std::vector<const Player*> m_players;
	int m_playerCount;
	const Player* m_current;
	bool m_valid, m_explicitPlayer;
};
