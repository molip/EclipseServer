#include "WSServer.h"
#include "HTMLServer.h"
#include "Controller.h"
#include "Players.h"
#include "Games.h"
#include "SaveThread.h"

int main(void) 
{
	Games::Load(); // Load games before players, so they can join their current game.
	Players::Load();

	Games::Verify();

	SaveThread savethread;
	Controller controller;
	WSServer serverWS(controller);
	HTMLServer serverHTML;
	getchar();
	return 0;
}
