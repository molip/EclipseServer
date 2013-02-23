// Copyright (c) 2004-2012 Sergey Lyubka
// This file is a part of mongoose project, http://github.com/valenok/mongoose

#include <cstdio>
#include <cstring>

#include "MongooseServer.h"
#include "mongoose.h"
#include "Util.h"

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
	ASSERT(m_mapPortToConn.insert(std::make_pair(port, pConn)).second);
}

void MongooseServer::Unregister(int port)
{
	LOCK(m_mutex);
	ASSERT(m_mapPortToConn.erase(port) == 1);
}

mg_connection* MongooseServer::FindConnection(int port) const
{
	LOCK(m_mutex);
	auto it = m_mapPortToConn.find(port);
	if (it != m_mapPortToConn.end())
		return it->second;
	return nullptr;
}

bool MongooseServer::SendMessage(int port, const std::string& msg) const
{
	auto pConn = FindConnection(port);
	if (!pConn)
		return false;

	std::string hdr;
	hdr.push_back((unsigned char)0x81);

	unsigned long long length = msg.size();
	int bytes = 0;
	if (length < 0x7e)
		bytes = 1;
	else if (length < 0x1000)
	{
		hdr.push_back(0x7e);
		bytes = 2;
	}
	else 
	{
		hdr.push_back(0x7f);
		bytes = 8;
	}

	for (int i = bytes - 1; i >= 0; --i)
		hdr.push_back(unsigned char(length >> i * 8));

	if (mg_write(pConn, hdr.c_str(), hdr.size()) < (int)hdr.size())
		return false;
		
	if (mg_write(pConn, msg.c_str(), msg.size()) < (int)msg.size())
		return false;
		
	return true;
}

static void *callback(enum mg_event event, struct mg_connection *conn) 
{
	//printf("Event received: %d, connection:%X\n", event,  conn);
	
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
		unsigned char buf[256];

		if (mg_read(conn, buf, 2) != 2) 
			return "";
		
		ASSERT(buf[0] == 0x81); // Text.
		ASSERT(buf[1] & 0x80);  // Encoded.

		int length = buf[1] & 0x7f; 
		if (int nExtra = length == 0x7f ? 8 : length == 0x7e ? 2 : 0) // Real length is in following bytes.
		{
			length = 0;
			if (mg_read(conn, buf, nExtra) != nExtra) 
				return "";
			for (int i = 0; i < nExtra; ++i)
				length += buf[i] << ((nExtra - 1 - i) * 8);
		}

		unsigned char mask[4];
		if (mg_read(conn, mask, 4) != 4) 
			return "";

		std::string msg(length, ' ');
		int read = 0;
		while (read < length)
		{
			int n = mg_read(conn, buf, sizeof buf);
			if (n <= 0)
				return ""; // Read error. 

			for (int i = 0; i < n; ++i)
				msg[read + i] = buf[i] ^ mask[i % 4];

			read += n;
		}

		pServer->OnMessage(request_info->remote_port, msg);
	} 
    return nullptr;
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

