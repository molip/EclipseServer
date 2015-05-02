#pragma once

#include "MongooseServer.h"

class Player;

class HTMLServer : public MongooseServer
{
public:
	HTMLServer();

	static const Player* Authenticate(const std::string& email, const std::string& password);
	static bool Authenticate(int playerId, const StringMap& cookies);

	virtual std::string OnHTTPRequest(const std::string& url, const std::string& host, const Request& request) override;

private:
};
