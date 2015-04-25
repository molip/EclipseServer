// Copyright (c) 2004-2012 Sergey Lyubka
// This file is a part of mongoose project, http://github.com/valenok/mongoose

#include <cstdio>
#include <cstring>

#include "MongooseServer.h"
#include "civetweb.h"
#include "Util.h"

static int begin_request_handler(struct mg_connection *conn);
static void connection_close_handler(mg_connection *conn);
static int websocket_connect_handler(const struct mg_connection *conn);
static void websocket_ready_handler(mg_connection *conn);
static int websocket_data_handler(mg_connection *conn, int flags, char *data, size_t data_len);

MongooseServer::MongooseServer(int port)
{
	char sPort[8];
	_itoa_s(port, sPort, 10);

	const char *options[] = 
	{
		"listening_ports", sPort,
		"document_root", "web",
		"request_timeout_ms", "60000",
		nullptr
	};

	mg_callbacks callbacks;
	memset(&callbacks, 0, sizeof callbacks);
	callbacks.begin_request = begin_request_handler;
	callbacks.connection_close = connection_close_handler;
	callbacks.websocket_connect = websocket_connect_handler;
	callbacks.websocket_ready = websocket_ready_handler;
	callbacks.websocket_data = websocket_data_handler;

	m_pContext = mg_start(&callbacks, this, options);
}
	
MongooseServer::~MongooseServer()
{
	mg_stop(m_pContext);
}

void MongooseServer::RegisterClient(ClientID client, mg_connection* pConn)
{
	LOCK(m_mutex);
	ASSERT(m_mapPortToConn.insert(std::make_pair(client, pConn)).second);
}

bool MongooseServer::UnregisterClient(ClientID client, bool bAbort)
{
	LOCK(m_mutex);

	auto it = m_mapPortToConn.find(client);
	if (it == m_mapPortToConn.end())
		return false;
	
	if (bAbort)
	{
		// Looks like mg_close_connection isn't thread safe. 
		m_setAbortConns.insert(it->second);
		mg_websocket_write(it->second, WEBSOCKET_OPCODE_PING, nullptr, 0); // Pong will force instant disconnect.
	}

	m_mapPortToConn.erase(it);
	return true;
}

bool MongooseServer::PopAbort(mg_connection* pConn)
{
	LOCK(m_mutex);
	return m_setAbortConns.erase(pConn) > 0;
}

mg_connection* MongooseServer::FindConnection(ClientID client) const
{
	LOCK(m_mutex);
	auto it = m_mapPortToConn.find(client);
	if (it != m_mapPortToConn.end())
		return it->second;
	return nullptr;
}

bool MongooseServer::SendMessage(ClientID client, const std::string& msg) const
{
	auto pConn = FindConnection(client);
	if (!pConn)
		return false;

	return mg_websocket_write(pConn, WEBSOCKET_OPCODE_TEXT, msg.c_str(), msg.size()) == msg.size();
}

std::string MongooseServer::CreateOKResponse(const std::string& content, const Cookies& cookies)
{
	return ::FormatString(
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: %0\r\n"        // Always set Content-Length
		"%2"
		"\r\n"
		"%1",
		content.size(), content.c_str(), cookies);
}

std::string MongooseServer::CreateRedirectResponse(const std::string& url, const Cookies& cookies)
{
	return ::FormatString(
		"HTTP/1.1 302 Found\r\n"
		"Location: %0\r\n"
		"%1"
		"\r\n",
		url, cookies);
}

std::string MongooseServer::CreateMD5(const std::string& string1, const std::string& string2)
{
	char buf[33];
	mg_md5(buf, string1.c_str(), string2.c_str(), nullptr);
	return buf;
}

class MongooseRequest : public IServer::Request
{
public:
	MongooseRequest(const mg_connection* conn) : m_conn(const_cast<mg_connection*>(conn)) {}

	virtual IServer::StringMap GetQueries() const override
	{
		IServer::StringMap queries;
		if (const char* queryString = mg_get_request_info(m_conn)->query_string)
			queries = IServer::SplitString(queryString, '&');
		return queries;
	}
	virtual IServer::StringMap GetCookies() const override
	{
		IServer::StringMap cookies;

		if (const char* cookiesString = mg_get_header(m_conn, "Cookie"))
			cookies = IServer::SplitString(cookiesString, ';');
		return cookies;
	}
	virtual IServer::StringMap GetPostData() const override
	{
		IServer::StringMap postData;
		char postDataString[1024];
		if (int bytes = mg_read(m_conn, postDataString, sizeof(postDataString)))
		{
			postDataString[bytes] = 0;
			postData = IServer::SplitString(postDataString, '&');
		}
		return postData;
	}
private:
	mg_connection* m_conn;
};

int begin_request_handler(struct mg_connection *conn)
{
	const mg_request_info *request_info = mg_get_request_info(conn);

	MongooseServer* pServer = reinterpret_cast<MongooseServer*>(request_info->user_data);
	const ClientID client = reinterpret_cast<ClientID>(conn);

	auto request = std::make_unique<MongooseRequest>(conn);

	std::string reply = pServer->OnHTTPRequest(request_info->uri, mg_get_header(conn, "Host"), *request);
	if (!reply.empty())
	{
		mg_write(conn, reply.c_str(), reply.size());
		return 1; // Mark as processed
	}
	return 0;
}

// Called on timeout.
void connection_close_handler(mg_connection *conn)
{
	const mg_request_info *request_info = mg_get_request_info(conn);
	MongooseServer* pServer = reinterpret_cast<MongooseServer*>(request_info->user_data);
	const ClientID client = reinterpret_cast<ClientID>(conn);

	if (pServer->UnregisterClient(client))
		pServer->OnWebSocketDisconnect(client);
}

int websocket_connect_handler(const struct mg_connection *conn)
{
	const mg_request_info *request_info = mg_get_request_info(const_cast<mg_connection*>(conn));
	MongooseServer* pServer = reinterpret_cast<MongooseServer*>(request_info->user_data);

	auto request = std::make_unique<MongooseRequest>(conn);
	return pServer->OnWebSocketConnect(request_info->uri, request->GetCookies()) ? 0 : 1;
}

void websocket_ready_handler(mg_connection *conn)
{
	const mg_request_info *request_info = mg_get_request_info(conn);
	MongooseServer* pServer = reinterpret_cast<MongooseServer*>(request_info->user_data);
	const ClientID client = reinterpret_cast<ClientID>(conn);

	pServer->RegisterClient(client, conn);
	pServer->OnWebSocketReady(client, request_info->uri);
}

int websocket_data_handler(mg_connection *conn, int flags, char *data, size_t data_len)
{
	const mg_request_info *request_info = mg_get_request_info(conn);
	MongooseServer* pServer = reinterpret_cast<MongooseServer*>(request_info->user_data);
	const ClientID client = reinterpret_cast<ClientID>(conn);

	if (pServer->PopAbort(conn))
		return 0;

	if (flags & 0x80)
	{
		flags &= 0x7f;
		switch (flags)
		{
		case WEBSOCKET_OPCODE_TEXT:
			if (data_len)
				pServer->OnWebSocketMessage(client, std::string(data, data_len));
			break;
		case WEBSOCKET_OPCODE_CONNECTION_CLOSE: // Called on refresh but not tab/browser close.
			pServer->UnregisterClient(client);
			pServer->OnWebSocketDisconnect(client);
			break;
		default:
			ASSERT(false);
		}
	}
	return 1;
}

//-----------------------------------------------------------------------------
//name = jon; id = 1
IServer::StringMap IServer::SplitString(const std::string& string, char sep)
{
	auto trim = [](const std::string& s)
	{
		int start = 0, end = (int)s.length();
		for (auto i = s.begin(); i != s.end() && *i == ' '; ++i)
			++start;
		if (start < end)
			for (auto i = s.rbegin(); i != s.rend() && *i == ' '; ++i)
				++end;
		return s.substr(start, end - start);
	};
	
	
	StringMap map;
	auto v = Util::SplitString(string, sep);

	for (auto& s : v)
	{
		auto pair = Util::SplitString(s, '=');
		if (pair.size() == 2)
			map[trim(pair[0])] = trim(pair[1]);
	}
	return map;
}

//-----------------------------------------------------------------------------

void IServer::Cookies::Set(const std::string& name, const std::string& value, bool httpOnly, int maxAge)
{
	std::string maxAgeString = maxAge >= 0 ? ::FormatString("; max-age=%0", maxAge) : "";
	std::string httpOnlyString = httpOnly ? "; HttpOnly" : "";

	*this += ::FormatString("Set-Cookie: %0=%1%2%3\r\n", name, value, maxAgeString, httpOnlyString);
}

void IServer::Cookies::Delete(const std::string& name)
{
	Set(name, "", false, 0);
}
