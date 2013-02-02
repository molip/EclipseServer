#pragma once

#include "App.h"

class IServer
{
public:
	typedef std::map<std::string, std::string> QueryMap;

	virtual ~IServer() {}
	virtual bool OnHTTPRequest(const std::string& url, const QueryMap& queries, std::string& reply) { return false; }
	virtual void OnConnect(int port, const std::string& url) {}
	virtual void OnDisconnect(int port) {}
	virtual void OnMessage(int port, const std::string& msg) {}

	static QueryMap SplitQuery(const std::string& query);
};

struct mg_context;
struct mg_connection;

class MongooseServer : public IServer
{
public:
	MongooseServer();
	virtual ~MongooseServer();

	void Register(int port, mg_connection* pConn);
	void Unregister(int port);
	bool SendMessage(int port, const std::string& msg);

private:
	mg_connection* MongooseServer::FindConnection(int port) const;

	mg_context* m_pContext;
	mutable std::mutex m_mutex;
	std::map<int, mg_connection*> m_mapPortToConn;
};
