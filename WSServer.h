#include "MongooseServer.h"

class WSServer : public MongooseServer
{
public:
	WSServer();
	virtual void OnConnect(int clientID, const std::string& url) override;
	virtual void OnMessage(int clientID, const std::string& message) override;
	virtual void OnDisconnect(int clientID) override;

private:
};
