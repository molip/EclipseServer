#pragma once

#include "MongooseServer.h"

class Player;

class HTMLServer : public MongooseServer
{
public:
	HTMLServer();

	static Player* Authenticate(const std::string& name, const std::string& password);
	static Player* Authenticate(const StringMap& cookies);

	virtual std::string OnHTTPRequest(const std::string& url, const std::string& host, const Request& request) override;

private:
};
