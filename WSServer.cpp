#include "WSServer.h"

#include "App.h"

WSServer::WSServer() : MongooseServer(8998)
{
}

void WSServer::OnConnect(int clientID, const std::string& url)
{
	std::ostringstream ss;
	ss << "Client " <<  clientID << " connected";
	SendMessage(clientID, ss.str());
	std::cout << ss.str() << std::endl;
}

void WSServer::OnMessage(int clientID, const std::string& message)
{
	std::ostringstream ss;
	ss << "Client " <<  clientID << " said:" << message;
	SendMessage(clientID, ss.str());
	std::cout << ss.str() << std::endl;

	// TODO: register client ID for this player/game
}

void WSServer::OnDisconnect(int clientID) 
{
	// TODO: call RemovePlayer
	std::cout << "Client " <<  clientID << " disconnected" << std::endl;
}
