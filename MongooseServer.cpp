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

int begin_request_handler(struct mg_connection *conn)
{
	const mg_request_info *request_info = mg_get_request_info(conn);

	MongooseServer* pServer = reinterpret_cast<MongooseServer*>(request_info->user_data);
	const ClientID client = reinterpret_cast<ClientID>(conn);

	IServer::QueryMap queries;
	if (request_info->query_string)
		queries = IServer::SplitQuery(request_info->query_string);

	std::string reply;
	if (pServer->OnHTTPRequest(request_info->uri, mg_get_header(conn, "Host"), queries, reply))
	{
		mg_printf(conn,
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: %d\r\n"        // Always set Content-Length
			"\r\n"
			"%s",
			reply.size(), reply.c_str());

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
		pServer->OnDisconnect(client);
}

int websocket_connect_handler(const struct mg_connection *)
{
	return 0;
}

void websocket_ready_handler(mg_connection *conn)
{
	const mg_request_info *request_info = mg_get_request_info(conn);
	MongooseServer* pServer = reinterpret_cast<MongooseServer*>(request_info->user_data);
	const ClientID client = reinterpret_cast<ClientID>(conn);

	pServer->RegisterClient(client, conn);
	pServer->OnConnect(client, request_info->uri);
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
				pServer->OnMessage(client, std::string(data, data_len));
			break;
		case WEBSOCKET_OPCODE_CONNECTION_CLOSE: // Called on refresh but not tab/browser close.
			pServer->UnregisterClient(client);
			pServer->OnDisconnect(client);
			break;
		default:
			ASSERT(false);
		}
	}
	return 1;
}

//-----------------------------------------------------------------------------

IServer::QueryMap IServer::SplitQuery(const std::string& query) 
{
	QueryMap map;

	auto v = Util::SplitString(query, '&');

	for (auto& s : v)
	{
		auto pair = Util::SplitString(s, '=');
		if (pair.size() == 2)
			map[pair[0]] = pair[1];
	}
	return map;
}

