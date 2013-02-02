#pragma once

#include <map>

class IServer
{
public:
	typedef std::map<std::string, std::string> QueryMap;

	virtual ~IServer() {}
	virtual bool OnHTTPRequest(const std::string& url, const QueryMap& queries, std::string& reply) { return false; }
	virtual void OnConnect(int clientID, const std::string& url) {}
	virtual void OnDisconnect(int clientID) {}
	virtual void OnMessage(int clientID, const std::string& msg) {}

	QueryMap SplitQuery(const std::string& query) const;
};

struct mg_context;
struct mg_connection;

class MongooseServer : public IServer
{
public:
	MongooseServer();
	virtual ~MongooseServer();

	void Register(int clientID, mg_connection* pConn);
	void Unregister(int clientID);
	bool SendMessage(int clientID, const std::string& msg);

private:
	mg_context* m_pContext;
	std::map<int, mg_connection*> m_mapClientToConn;
	//std::map<mg_connection*, int> m_mapConnToClient;
};
