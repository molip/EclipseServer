#pragma once

#include "MongooseServer.h"

#include <map>

class Controller;
namespace Output { class Message; }

class WSServer : public MongooseServer
{
public:
	WSServer(Controller& controller);
	virtual void OnConnect(int port, const std::string& url) override;
	virtual void OnMessage(int port, const std::string& message) override;
	virtual void OnDisconnect(int port) override;

	bool SendMessage(const Output::Message& msg, const std::string& player = "") const;
	bool SendMessage(const std::string& msg, const std::string& player = "") const;
	void BroadcastMessage(const std::string& msg) const;

private:
	void RegisterPlayer(int port, const std::string& player);
	void ReportError(const std::string& type, const std::string& msg, int port = 0);

	std::map<int, std::string> m_mapPortToPlayer;
	std::map<std::string, int> m_mapPlayerToPort;
	mutable std::mutex m_mutex;

	Controller& m_controller;
};
