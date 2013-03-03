#pragma once

#include "MongooseServer.h"

#include <map>

class Controller;
namespace Output { class Message; }

class WSServer : public MongooseServer
{
public:
	WSServer(Controller& controller);
	virtual void OnConnect(ClientID client, const std::string& url) override;
	virtual void OnMessage(ClientID client, const std::string& message) override;
	virtual void OnDisconnect(ClientID client) override;

	bool SendMessage(const Output::Message& msg, const std::string& player) const;
	bool SendMessage(const std::string& msg, const std::string& player) const;
	void BroadcastMessage(const Output::Message& msg) const;
	void BroadcastMessage(const std::string& msg) const;

private:
	void RegisterPlayer(ClientID client, const std::string& player);
	void ReportError(const std::string& type, const std::string& msg, ClientID client = 0);

	std::map<ClientID, std::string> m_mapClientToPlayer;
	std::map<std::string, ClientID> m_mapPlayerToClient;
	mutable std::mutex m_mutex;

	Controller& m_controller;
};
