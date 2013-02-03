#include "MongooseServer.h"

class HTMLServer : public MongooseServer
{
public:
	HTMLServer();
	virtual bool OnHTTPRequest(const std::string& url, const QueryMap& queries, std::string& reply) override;

private:
	std::string GetLobbyHTML() const;
	std::string GetGameHTML(const std::string& game, const std::string& player) const;
};
