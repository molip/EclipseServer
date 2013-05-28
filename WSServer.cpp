#include "WSServer.h"
#include "App.h"
#include "Util.h"
#include "Controller.h"
#include "Output.h"
#include "Input.h"
#include "Player.h"
#include "Players.h"

WSServer::WSServer(Controller& controller) : MongooseServer(8998), m_controller(controller)
{
	controller.SetServer(this);
}

void WSServer::OnConnect(ClientID client, const std::string& url)
{
	LOCK(m_mutex); 
	std::cout << "INFO: Client connected: " << client << std::endl;
}

void WSServer::OnMessage(ClientID client, const std::string& message)
{
	LOCK(m_mutex); // Only process 1 message at a time. 

	try 
	{
		if (const Input::MessagePtr pMsg = Input::CreateMessage(message))
		{
			if (auto pRegister = dynamic_cast<const Input::Register*>(pMsg.get()))
			{
				RegisterPlayer(client, Players::Get(pRegister->GetPlayerID()));
			}
			else
			{
				auto i = m_mapClientToPlayer.find(client);
				if (i != m_mapClientToPlayer.end())
					m_controller.OnMessage(pMsg, *i->second);
			}
		}
		else
			AssertThrow("OnMessage");
	}
	catch (Exception& e)
	{
		ReportError(e.GetType(), e.what(), client);
	}
}

void WSServer::RegisterPlayer(ClientID client, Player& player)
{
	auto i = m_mapPlayerToClient.find(&player);
	if (i != m_mapPlayerToClient.end())
	{
		__super::SendMessage(client, "ERROR:DUPLICATE_PLAYER");
		return;
	}
		
	m_mapPlayerToClient[&player] = client;
	m_mapClientToPlayer[client] = &player;

	std::cout << "INFO: Client registered: " << client << " -> " << player.GetName() << std::endl;
	m_controller.OnPlayerConnected(player);
}

void WSServer::OnDisconnect(ClientID client) 
{
	LOCK(m_mutex); 

	auto i = m_mapClientToPlayer.find(client);
	if (i == m_mapClientToPlayer.end())
		std::cerr << "ERROR: Unregistered client disconnected: " << client << std::endl;
	else
	{
		std::cout << "INFO: Client disconnected: " << client << std::endl;
		m_controller.OnPlayerDisconnected(*i->second);
		m_mapPlayerToClient.erase(i->second);
		m_mapClientToPlayer.erase(client);
	}
}

bool WSServer::SendMessage(const Output::Message& msg, const Player& player) const
{
	return SendMessage(msg.GetXML(), player);
}

void WSServer::BroadcastMessage(const Output::Message& msg) const
{
	return BroadcastMessage(msg.GetXML());
}

bool WSServer::SendMessage(const std::string& msg, const Player& player) const
{
	auto i = m_mapPlayerToClient.find(const_cast<Player*>(&player));
	if (i == m_mapPlayerToClient.end())
		return false;

	__super::SendMessage(i->second, msg);

	return true;
}

void WSServer::BroadcastMessage(const std::string& msg) const
{
	for (auto& i : m_mapClientToPlayer)
		__super::SendMessage(i.first, msg);
}

void WSServer::ReportError(const std::string& type, const std::string& msg, ClientID client)
{
	std::ostringstream ss;
	ss << "Exception : " << type << " : " << msg;
	if (client)
	{
		ss << " [" << client;

		auto i = m_mapClientToPlayer.find(client);
		if (i != m_mapClientToPlayer.end())
			ss << " " << i->second;

		ss << "]";
	}
	std::cerr << ss.str() << std::endl;
}
