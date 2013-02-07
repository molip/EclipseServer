#include "HTMLServer.h"

#include <fstream>

#include "App.h"
#include "Game.h"

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
	std::string GetGameURL(const std::string& game, const std::string& player)
	{
		std::ostringstream ss;
		ss << "/game?game=" << game << "&player=" << player;
		return ss.str();
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

HTMLServer::HTMLServer() : MongooseServer(80)
{
}

bool HTMLServer::OnHTTPRequest(const std::string& url, const QueryMap& queries, std::string& reply)
{
	if (url == "/game")
	{
		auto pid = queries.find("player");
		if (pid != queries.end())
		{
			std::string sPage = LoadFile("web\\game.html");
			ReplaceToken(sPage, "%PLAYER%", pid->second);
			reply = sPage;
			return true;
		}
		reply = "No player name found";
		return true;
	}
	return false; 
}
