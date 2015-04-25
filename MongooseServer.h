#pragma once

#include "App.h"

typedef unsigned long long ClientID;

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
		int GetInt(const std::string& name) const
		{
			return std::atoi(Get(name).c_str());
		}
	};

	class Cookies : public std::string
	{
	public:
		void Set(const std::string& name, const std::string& value, bool httpOnly = false, int maxAge = -1);
		void Delete(const std::string& name);
	};

	class Request
	{
	public:
		virtual StringMap GetQueries() const = 0;
		virtual StringMap GetCookies() const = 0;
		virtual StringMap GetPostData() const = 0;
	};

	virtual ~IServer() {}
	virtual std::string OnHTTPRequest(const std::string& url, const std::string& host, const Request& request) { return ""; }
	virtual bool OnWebSocketConnect(const std::string& url, const StringMap& cookies) { return true; }
	virtual void OnWebSocketReady(ClientID client, const std::string& url) {}
	virtual void OnWebSocketDisconnect(ClientID client) {}
	virtual void OnWebSocketMessage(ClientID client, const std::string& msg) {}

	static StringMap SplitString(const std::string& string, char sep);
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
	
	static std::string CreateOKResponse(const std::string& content, const Cookies& cookies = Cookies());
	static std::string CreateRedirectResponse(const std::string& newUrl, const Cookies& cookies = Cookies());

	static std::string CreateMD5(const std::string& string1, const std::string& string2);

protected:
	bool AbortClient(ClientID client);

private:
	mg_connection* FindConnection(ClientID client) const;

	mg_context* m_pContext;
	mutable std::mutex m_mutex;
	std::map<ClientID, mg_connection*> m_mapPortToConn;
	std::set<mg_connection*> m_setAbortConns;
};
