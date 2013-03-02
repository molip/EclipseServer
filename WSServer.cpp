#include "WSServer.h"
#include "App.h"
#include "Util.h"
#include "Controller.h"
#include "Output.h"
#include "Input.h"

WSServer::WSServer(Controller& controller) : MongooseServer(8998), m_controller(controller)
{
	controller.SetServer(this);
}

void WSServer::OnConnect(int port, const std::string& url)
{
	LOCK(m_mutex); 
	std::cout << "INFO: Port connected: " << port << std::endl;
}

void WSServer::OnMessage(int port, const std::string& message)
{
	LOCK(m_mutex); // Only process 1 message at a time. 

	try 
	{
		if (const Input::MessagePtr pMsg = Input::CreateMessage(message))
		{
			if (auto pRegister = dynamic_cast<const Input::Register*>(pMsg.get()))
			{
				RegisterPlayer(port, pRegister->GetPlayer());
			}
			else
			{
				auto i = m_mapPortToPlayer.find(port);
				if (i != m_mapPortToPlayer.end())
					m_controller.OnMessage(pMsg, i->second);
			}
		}
		else
			AssertThrow("OnMessage");
	}
	catch (Exception& e)
	{
		ReportError(e.GetType(), e.what(), port);
	}
}

void WSServer::RegisterPlayer(int port, const std::string& player)
{
	auto i = m_mapPlayerToPort.find(player);
	if (i != m_mapPlayerToPort.end())
	{
		__super::SendMessage(port, "ERROR:DUPLICATE_PLAYER");
		return;
	}
		
	m_mapPlayerToPort[player] = port;
	m_mapPortToPlayer[port] = player;

	std::cout << "INFO: Port registered: " << port << " -> " << player << std::endl;
	m_controller.OnPlayerConnected(player);
}

void WSServer::OnDisconnect(int port) 
{
	LOCK(m_mutex); 

	auto i = m_mapPortToPlayer.find(port);
	if (i == m_mapPortToPlayer.end())
		std::cerr << "ERROR: Unregistered port disconnected: " << port << std::endl;
	else
	{
		std::cout << "INFO: Port disconnected: " << port << std::endl;
		m_controller.OnPlayerDisconnected(i->second);
		m_mapPlayerToPort.erase(i->second);
		m_mapPortToPlayer.erase(port);
	}
}

bool WSServer::SendMessage(const Output::Message& msg, const std::string& player) const
{
	return SendMessage(msg.GetXML(), player);
}

bool WSServer::SendMessage(const std::string& msg, const std::string& player) const
{
	if (player.empty())
		BroadcastMessage(msg);
	else
	{
		auto i = m_mapPlayerToPort.find(player);
		if (i == m_mapPlayerToPort.end())
			return false;

		__super::SendMessage(i->second, msg);
	}
	return true;
}

void WSServer::BroadcastMessage(const std::string& msg) const
{
	for (auto i : m_mapPortToPlayer)
		__super::SendMessage(i.first, msg);
}

void WSServer::ReportError(const std::string& type, const std::string& msg, int port)
{
	std::ostringstream ss;
	ss << "Exception : " << type << " : " << msg;
	if (port)
	{
		ss << " [" << port;

		auto i = m_mapPortToPlayer.find(port);
		if (i != m_mapPortToPlayer.end())
			ss << " " << i->second;

		ss << "]";
	}
	std::cerr << ss.str() << std::endl;
}
