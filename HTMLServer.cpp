#include "HTMLServer.h"

#include <fstream>

#include "App.h"
#include "Game.h"
#include "Controller.h"
#include "Model.h"

namespace
{
	std::string LoadFile(const char* path)
	{
		std::string s, line;
		std::ifstream file(path);
		if (file.is_open())
		{
			std::stringstream buffer;
			buffer << file.rdbuf();
			s = buffer.str();
			file.close();
		}
		return s;
	}

	std::string GetRedirectHTML(const std::string& url)
	{
		std::string html =	"<!DOCTYPE HTML>"
							"<html>"
							"<head>"
							"<meta http-equiv=\"REFRESH\" content=\"0;url=";
		html += url;
		html +=				"\">"
							"</head>"
							"<body/>"
							"</html>";
		return html;
	}

	bool ReplaceToken(std::string& str, const std::string& token, const std::string& real)
	{
		size_t pos = str.find(token);
		if (pos == std::string::npos)
			return false;
		str.replace(pos, token.size(), real);
		return true;
	}
}

typedef std::unique_ptr<Game> GamePtr;
std::vector<GamePtr> games;

HTMLServer::HTMLServer() : MongooseServer(8999)
{
}

bool HTMLServer::OnHTTPRequest(const std::string& url, const std::string& host, const QueryMap& queries, std::string& reply)
{
	if (url == "/game")
	{
		auto pid = queries.find("player"); // TODO: Authentication.
		if (pid != queries.end())
		{
			const std::string& name = pid->second;
			if (const Player* pPlayer = Controller::Get()->GetModel().FindPlayer(name))
			{
				ASSERT(host.substr(host.size() - 5) == ":8999");
				std::string wsURL = std::string("ws://") + host.substr(0, host.size() - 4) + "8998";
			
				std::string sPage = LoadFile("web\\game.html");
				ReplaceToken(sPage, "%PLAYER_ID%", FormatInt(pPlayer->GetID()));
				ReplaceToken(sPage, "%WSURL%", wsURL);

				reply = sPage;
				return true;
			}
		}
		reply = "Player name not recognised";
		return true;
	}
	return false; 
}
