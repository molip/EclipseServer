#pragma once

#include "MongooseServer.h"

class HTMLServer : public MongooseServer
{
public:
	HTMLServer();

	static bool Authenticate(const std::string& name, const std::string& password);
	static bool Authenticate(const StringMap& cookies);

	virtual std::string OnHTTPRequest(const std::string& url, const std::string& host, const Request& request) override;

private:
};
