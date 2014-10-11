#include "WSServer.h"
#include "HTMLServer.h"
#include "Controller.h"
#include "Players.h"
#include "Games.h"
#include "SaveThread.h"

int main(void) 
{
	Players::Add("jon", "noj");	// 1
	Players::Add("alex", "xela");	// 2
	Players::Add("ben", "neb");	// 3
	Players::Add("mark", "kram");	// 4
	Players::Add("boris", "sirob");	// 5

	Games::Load();

	SaveThread savethread;
	Controller controller;
	WSServer serverWS(controller);
	HTMLServer serverHTML;
	getchar();
	return 0;
}
