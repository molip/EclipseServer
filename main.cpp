#include "WSServer.h"
#include "HTMLServer.h"
#include "Controller.h"
#include "Players.h"
#include "Games.h"
#include "SaveThread.h"

int main(void) 
{
	Players::Load();
	Games::Load(); 

	Players::RejoinCurrentGame();

	SaveThread savethread;
	Controller controller;
	WSServer serverWS(controller);
	HTMLServer serverHTML;
	getchar();
	return 0;
}
