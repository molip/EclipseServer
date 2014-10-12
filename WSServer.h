#pragma once

#include "MongooseServer.h"

#include <map>

class Controller;
class Player;

namespace Output { class Message; }

class WSServer : public MongooseServer
{
public:
	WSServer(Controller& controller);
	virtual bool OnWebSocketConnect(const std::string& url, const StringMap& cookies) override;
	virtual void OnWebSocketReady(ClientID client, const std::string& url) override;
	virtual void OnWebSocketMessage(ClientID client, const std::string& message) override;
	virtual void OnWebSocketDisconnect(ClientID client) override;

	bool SendMessage(const Output::Message& msg, const Player& player) const;
	bool SendMessage(const std::string& msg, const Player& player) const;
	void BroadcastMessage(const Output::Message& msg) const;
	void BroadcastMessage(const std::string& msg) const;

	const std::set<Player*>& GetPlayers() const { return m_players; }

private:
	void RegisterPlayer(ClientID client, Player& player);
	void UnregisterPlayer(ClientID client);
	std::string GetErrorMessage(const std::string& type, const std::string& msg, ClientID client = 0);

	std::map<ClientID, Player*> m_mapClientToPlayer;
	std::map<Player*, ClientID> m_mapPlayerToClient;
	std::set<Player*> m_players;
	mutable std::mutex m_mutex;

	Controller& m_controller;
};
