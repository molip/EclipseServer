#include "MongooseServer.h"

class Server : public MongooseServer
{
public:
	Server();
	virtual bool OnHTTPRequest(const std::string& url, const QueryMap& queries, std::string& reply) override;
	virtual void OnConnect(int clientID, const std::string& url) override;
	virtual void OnMessage(int clientID, const std::string& message) override;
	virtual void OnDisconnect(int clientID) override;

private:
	std::string GetLobbyHTML() const;
	std::string GetGameHTML(const std::string& game, const std::string& player) const;

	std::string m_pageLobby, m_pageGame;
};
