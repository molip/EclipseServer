#pragma once

#include "MongooseServer.h"

class HTMLServer : public MongooseServer
{
public:
	HTMLServer();
	virtual bool OnHTTPRequest(const std::string& url, const std::string& host, const QueryMap& queries, std::string& reply) override;

private:
};
