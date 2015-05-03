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
	std::vector<const Player*> GetPlayers() const;

	void Add(Player& player);
	void RemoveCurrent();

private:
	typedef std::vector<std::pair<Player*, std::string>> PlayerVec;

	PlayerVec::const_iterator Find(const Player& player) const;

	PlayerVec m_players; // Player, session id.
	int m_playerCount;
	const Player* m_current;
	bool m_valid, m_explicitPlayer;
};
