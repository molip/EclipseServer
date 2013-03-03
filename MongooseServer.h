#pragma once

#include "App.h"

typedef unsigned long ClientID;

class IServer
{
public:
	typedef std::map<std::string, std::string> QueryMap;

	virtual ~IServer() {}
	virtual bool OnHTTPRequest(const std::string& url, const std::string& host, const QueryMap& queries, std::string& reply) { return false; }
	virtual void OnConnect(ClientID client, const std::string& url) {}
	virtual void OnDisconnect(ClientID client) {}
	virtual void OnMessage(ClientID client, const std::string& msg) {}

	static QueryMap SplitQuery(const std::string& query);
};

struct mg_context;
struct mg_connection;

class MongooseServer : public IServer
{
public:
	MongooseServer(int port);
	virtual ~MongooseServer();

	void Register(ClientID client, mg_connection* pConn);
	void Unregister(ClientID client);
	bool SendMessage(ClientID client, const std::string& msg) const;

private:
	mg_connection* MongooseServer::FindConnection(ClientID client) const;

	mg_context* m_pContext;
	mutable std::mutex m_mutex;
	std::map<ClientID, mg_connection*> m_mapPortToConn;
};
