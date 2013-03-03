#include "WSServer.h"
#include "HTMLServer.h"
#include "Model.h"
#include "Controller.h"

int main(void) 
{
	::srand(unsigned(::time(nullptr)));
	
	Model model;
	Controller controller(model);
	WSServer serverWS(controller);
	HTMLServer serverHTML;
	getchar();
	return 0;
}
