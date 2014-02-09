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

	void RegisterClient(ClientID client, mg_connection* pConn);
	bool UnregisterClient(ClientID client, bool bAbort = false);
	bool SendMessage(ClientID client, const std::string& msg) const;
	bool PopAbort(mg_connection* pConn);

protected:
	bool AbortClient(ClientID client);

private:
	mg_connection* FindConnection(ClientID client) const;

	mg_context* m_pContext;
	mutable std::mutex m_mutex;
	std::map<ClientID, mg_connection*> m_mapPortToConn;
	std::set<mg_connection*> m_setAbortConns;
};
