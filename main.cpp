#include "WSServer.h"
#include "HTMLServer.h"
#include "Controller.h"
#include "Players.h"

int main(void) 
{
	Players::Add("jon", "noj");	// 1
	Players::Add("alex", "xela");	// 2
	Players::Add("ben", "neb");	// 3

	Controller controller;
	WSServer serverWS(controller);
	HTMLServer serverHTML;
	getchar();
	return 0;
}
