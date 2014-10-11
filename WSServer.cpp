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
	std::cout << "INFO: Client connected: " << client << std::endl;
}

void WSServer::OnMessage(ClientID client, const std::string& message)
{
	LOCK(m_mutex); // Only process 1 message at a time. 

	Player* player = nullptr;
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
				{
					player = i->second;
					m_controller.OnMessage(pMsg, *player);
				}
			}
		}
		else
			VERIFY_INPUT(false);
	}
	catch (Exception& e)
	{
		std::string error = GetErrorMessage(e.GetTypeString(), e.what(), client);
		std::cerr << error << std::endl;

		if (player)
		{
			if (const Game* game = player->GetCurrentGame())
			{
				m_controller.ClearQueuedMessages();
				//m_controller.SendUpdateGame(*game); // Uh oh! Can trigger exceptionception.
				m_controller.SendMessage(Output::AddLog(0, error), *game);
				m_controller.SendQueuedMessages();
			}
		}
	}

	if (player)
		SendMessage(Output::Response(), *player);
}

void WSServer::OnDisconnect(ClientID client)
{
	LOCK(m_mutex);
	UnregisterPlayer(client);
}

void WSServer::RegisterPlayer(ClientID client, Player& player)
{
	auto i = m_mapPlayerToClient.find(&player);
	if (i != m_mapPlayerToClient.end())
		if (UnregisterClient(i->second, true))
			UnregisterPlayer(i->second);
		
	m_mapPlayerToClient[&player] = client;
	m_mapClientToPlayer[client] = &player;
	m_players.insert(&player);

	std::cout << "INFO: Client registered: " << client << " -> " << player.GetName() << std::endl;
	m_controller.OnPlayerConnected(player);
}


void WSServer::UnregisterPlayer(ClientID client)
{
	auto i = m_mapClientToPlayer.find(client);
	if (i == m_mapClientToPlayer.end())
		std::cerr << "ERROR: Unregistered client disconnected: " << client << std::endl;
	else
	{
		Player* pPlayer = i->second;
		std::cout << "INFO: Client disconnected: " << client << std::endl;
		m_controller.OnPlayerDisconnected(*pPlayer);
		m_mapPlayerToClient.erase(pPlayer);
		m_mapClientToPlayer.erase(client);
		m_players.erase(pPlayer);
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

std::string WSServer::GetErrorMessage(const std::string& type, const std::string& msg, ClientID client)
{
	std::ostringstream ss;
	ss << type << " exception: " << msg;
	std::string name;
	if (client)
	{
		ss << " [client: ";

		auto i = m_mapClientToPlayer.find(client);
		if (i != m_mapClientToPlayer.end())
			ss << i->second->GetName();
		else
			ss << client;

		ss << "]";
	}
	return ss.str();
}
