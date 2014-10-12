#pragma once

#include "App.h"

typedef unsigned long ClientID;

class IServer
{
public:
	struct StringMap : std::map<std::string, std::string>
	{
		std::string Get(const std::string& name) const
		{
			auto it = find(name);
			return it == end() ? "" : it->second;
		}
	};

	virtual ~IServer() {}
	virtual std::string OnHTTPRequest(const std::string& url, const std::string& host, const StringMap& queries, const StringMap& cookies) { return ""; }
	virtual bool OnWebSocketConnect(const std::string& url, const StringMap& cookies) { return true; }
	virtual void OnWebSocketReady(ClientID client, const std::string& url) {}
	virtual void OnWebSocketDisconnect(ClientID client) {}
	virtual void OnWebSocketMessage(ClientID client, const std::string& msg) {}

	static StringMap SplitString(const std::string& string, char sep);
};

struct mg_context;
struct mg_connection;

class Cookies : public std::string
{
public:
	void Set(const std::string& name, const std::string& value, bool httpOnly = false, int maxAge = -1);
	void Delete(const std::string& name);
};

class Request
{
public:
	void Set(const std::string& name, const std::string& value, bool httpOnly = false, int maxAge = -1);
	void Delete(const std::string& name);
};

class MongooseServer : public IServer
{
public:
	MongooseServer(int port);
	virtual ~MongooseServer();

	void RegisterClient(ClientID client, mg_connection* pConn);
	bool UnregisterClient(ClientID client, bool bAbort = false);
	bool SendMessage(ClientID client, const std::string& msg) const;
	bool PopAbort(mg_connection* pConn);

	std::string CreateOKResponse(const std::string& content, const Cookies& cookies = Cookies()) const;
	std::string CreateRedirectResponse(const std::string& newUrl, const Cookies& cookies = Cookies()) const;

protected:
	bool AbortClient(ClientID client);

private:
	mg_connection* FindConnection(ClientID client) const;

	mg_context* m_pContext;
	mutable std::mutex m_mutex;
	std::map<ClientID, mg_connection*> m_mapPortToConn;
	std::set<mg_connection*> m_setAbortConns;
};
