#include "HTMLServer.h"

#include <fstream>

#include "App.h"
#include "Game.h"
#include "Controller.h"
#include "Players.h"

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

bool HTMLServer::Authenticate(const std::string& name, const std::string& password)
{
	const Player* pPlayer = Players::Find(name);
	return pPlayer && pPlayer->GetPassword() == password;
}

bool HTMLServer::Authenticate(const StringMap& cookies)
{
	return Authenticate(cookies.Get("name"), cookies.Get("password"));
}

std::string HTMLServer::OnHTTPRequest(const std::string& url, const std::string& host, const Request& request)
{
	auto cookies = request.GetCookies();

	if (url == "/login")
	{
		auto postData = request.GetPostData();

		auto name = postData.Get("player");
		auto password = postData.Get("password");

		if (Authenticate(name, password))
		{
			Cookies newCookies;
			newCookies.Set("name", name, true);
			newCookies.Set("password", password, true);
			return CreateRedirectResponse("/", newCookies);
		}

		return CreateRedirectResponse("/login.html?failed=1");
	}

	if (url != "/login.html")
		if (!Authenticate(cookies))
			return CreateRedirectResponse("/login.html");

	if (url == "/")
	{
		const std::string& name = cookies.Get("name");
		if (const Player* pPlayer = Players::Find(name))
		{
			ASSERT(host.substr(host.size() - 5) == ":8999");
			std::string wsURL = std::string("ws://") + host.substr(0, host.size() - 4) + "8998";
			
			std::string sPage = LoadFile("web\\game.html");
			ReplaceToken(sPage, "%PLAYER_ID%", FormatInt(pPlayer->GetID()));
			ReplaceToken(sPage, "%WSURL%", wsURL);

			return CreateOKResponse(sPage);
		}
		return CreateRedirectResponse("/login.html");
	}
	return ""; 
}
