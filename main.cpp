#include "stdafx.h"
#include "WSServer.h"
#include "HTMLServer.h"
#include "Controller.h"
#include "Players.h"
#include "Games.h"
#include "SaveThread.h"
#include "Test.h"

int main(void) 
{
	Players::Load();
	Games::Load(); 

	Test::Run();

	Players::RejoinCurrentGame();

	SaveThread savethread;
	Controller controller;

	std::unique_ptr<WSServer> serverWS;
	std::unique_ptr<HTMLServer> serverHTML;

	try
	{
		serverWS = std::make_unique<WSServer>(controller);
		serverHTML = std::make_unique<HTMLServer>();
	}
	catch (const std::runtime_error&)
	{
	}
	
	getchar();
	return 0;
}
