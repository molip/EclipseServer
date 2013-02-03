// Copyright (c) 2004-2012 Sergey Lyubka
// This file is a part of mongoose project, http://github.com/valenok/mongoose

#include <cstdio>
#include <cstring>

#include "MongooseServer.h"
#include "mongoose.h"

static void *callback(enum mg_event event, struct mg_connection *conn);

MongooseServer::MongooseServer(int port)
{
	char sPort[8];
	_itoa_s(port, sPort, 10);

	const char *options[] = 
	{
		"listening_ports", sPort,
		"document_root", "web",
		nullptr
	};

	m_pContext = mg_start(&callback, this, options);
}
	
MongooseServer::~MongooseServer()
{
	mg_stop(m_pContext);
}

void MongooseServer::Register(int port, mg_connection* pConn)
{
	LOCK(m_mutex);
	assert(m_mapPortToConn.insert(std::make_pair(port, pConn)).second);
}

void MongooseServer::Unregister(int port)
{
	LOCK(m_mutex);
	assert(m_mapPortToConn.erase(port) == 1);
}

mg_connection* MongooseServer::FindConnection(int port) const
{
	LOCK(m_mutex);
	auto it = m_mapPortToConn.find(port);
	if (it != m_mapPortToConn.end())
		return it->second;
	return nullptr;
}

bool MongooseServer::SendMessage(int port, const std::string& msg)
{
	if (auto pConn = FindConnection(port))
	{
		std::string frame;
		frame.push_back((char)0x81);
		frame.push_back(msg.size());
		frame += msg;

		return mg_write(pConn, frame.c_str(), frame.size()) > 0;
	}
	return false;
}

static void *callback(enum mg_event event, struct mg_connection *conn) 
{
	printf("Event received: %d, connection:%X\n", event,  conn);
	
	const struct mg_request_info *request_info = mg_get_request_info(conn);
	
	MongooseServer* pServer = reinterpret_cast<MongooseServer*>(request_info->user_data);

	if (event == MG_NEW_REQUEST) 
	{
		IServer::QueryMap queries;
		if (request_info->query_string)
			queries = IServer::SplitQuery(request_info->query_string);

		std::string reply;
		if (pServer->OnHTTPRequest(request_info->uri, queries, reply))
		{
			mg_printf(conn,
					  "HTTP/1.1 200 OK\r\n"
					  "Content-Type: text/html\r\n"
					  "Content-Length: %d\r\n"        // Always set Content-Length
					  "\r\n"
					  "%s",
					  reply.size(), reply.c_str());
			
			return ""; // Mark as processed
		}
		return nullptr;
	}
	else if (event == MG_WEBSOCKET_READY) 
	{
		pServer->Register(request_info->remote_port, conn);
		pServer->OnConnect(request_info->remote_port, request_info->uri);
	}
	else if (event == MG_WEBSOCKET_CLOSE) 
	{
		pServer->Unregister(request_info->remote_port);
		pServer->OnDisconnect(request_info->remote_port);
	}
	else if (event == MG_WEBSOCKET_MESSAGE) 
	{
		unsigned char buf[200];
		int n, i, mask_len, xor, msg_len, len;

		// Read message from the client.
		// Accept only small (<126 bytes) messages.
		len = 0;
		msg_len = mask_len = 0;
		for (;;) 
		{
			if ((n = mg_read(conn, buf + len, sizeof(buf) - len)) <= 0) 
				return ""; // Read error, close websocket

			len += n;
			if (len >= 2) 
			{
				msg_len = buf[1] & 127;
				mask_len = (buf[1] & 128) ? 4 : 0;
				if (msg_len > 125) 
					return ""; // Message is too long, close websocket

				// If we've buffered the whole message, exit the loop
				if (len >= 2 + mask_len + msg_len) 
					break;
			}
		}

		// Copy message to string, applying the mask if required.
		std::string msg;
		msg.reserve(msg_len);
		for (i = 0; i < msg_len; i++) 
		{
			xor = mask_len == 0 ? 0 : buf[2 + (i % 4)];
			msg.push_back(buf[i + 2 + mask_len] ^ xor);
		}

		pServer->OnMessage(request_info->remote_port, msg);
	} 
    return nullptr;
}

//-----------------------------------------------------------------------------

IServer::QueryMap IServer::SplitQuery(const std::string& query) 
{
	QueryMap map;

	std::string var, val;
	bool bVal = false;
	for (char c : query)
	{
		if (c == '=')
			bVal = true;
		else if (c == '&')
		{
			bVal = false;
			if (!var.empty())
				map[var] = val;
			var.clear();
			val.clear();
		}
		else
			(bVal ? val : var).push_back(c);
	}
	if (!var.empty())
		map[var] = val;

	return map;
}

