#pragma once

#include "MongooseServer.h"

class HTMLServer : public MongooseServer
{
public:
	HTMLServer();
	virtual std::string OnHTTPRequest(const std::string& url, const std::string& host, const StringMap& queries, const StringMap& cookies) override;

private:
};
