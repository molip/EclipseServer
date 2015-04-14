#include "HTMLServer.h"

#include <fstream>

#include "App.h"
#include "Game.h"
#include "Controller.h"
#include "Players.h"
#include "Invitations.h"
#include "Util.h"

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
}

typedef std::unique_ptr<Game> GamePtr;
std::vector<GamePtr> games;

HTMLServer::HTMLServer() : MongooseServer(8999)
{
}

Player* HTMLServer::Authenticate(const std::string& name, const std::string& password)
{
	Player* pPlayer = Players::Find(name);
	return pPlayer && pPlayer->CheckPassword(password) ? pPlayer : nullptr;
}

Player* HTMLServer::Authenticate(const StringMap& cookies)
{
	Player* pPlayer = Players::Find(cookies.Get("name"));
	return pPlayer && pPlayer->GetPasswordHash() == cookies.Get("session") ? pPlayer : nullptr;
}

std::string HTMLServer::OnHTTPRequest(const std::string& url, const std::string& host, const Request& request)
{
	auto cookies = request.GetCookies();

	if (url == "/login")
	{
		auto postData = request.GetPostData();

		auto name = postData.Get("player");
		auto password = postData.Get("password");

		if (Player* player = Authenticate(name, password))
		{
			Cookies newCookies;
			newCookies.Set("name", name);
			newCookies.Set("session", player->GetPasswordHash(), true);
			return CreateRedirectResponse("/", newCookies);
		}

		return CreateRedirectResponse("/login.html?failed=1");
	}
	
	if (url == "/logout")
	{
		if (const Player* player = Authenticate(cookies))
		{
			Cookies newCookies;
			newCookies.Delete("name");
			newCookies.Delete("session");
			return CreateRedirectResponse("/", newCookies);
		}
	}

	if (url == "/register")
	{
		auto postData = request.GetPostData();

		auto name = postData.Get("player");
		auto password = postData.Get("password1");
		auto code = postData.Get("code");

		auto redirect = [&](const std::string& error)
		{
			const std::string& errorParam = error.empty() ? "" : ::FormatString("&error=%0", error);
			return CreateRedirectResponse(::FormatString("/register.html?code=%0%1", code, errorParam));
		};

		if (name.empty() || password.empty()) // Shouldn't happen, checked client-side.
			return redirect("");
		
		if (Players::Find(name))
			return redirect("name_taken");

		if (!Invitations::Find(code))
			return redirect("invalid_code");

		Player& player = Players::Add(name, password);
		Invitations::Remove(code);

		Cookies newCookies;
		newCookies.Set("name", name, true);
		newCookies.Set("session", player.GetPasswordHash(), true);
		return CreateRedirectResponse("/", newCookies);
	}

	const Player* player = nullptr;


	if (url != "/login.html" && url != "/register.html" && url != "/util.js")
		if (!(player = Authenticate(cookies)))
			return CreateRedirectResponse("/login.html");

	if (url == "/")
	{
		ASSERT(host.substr(host.size() - 5) == ":8999");
		std::string wsURL = std::string("ws://") + host.substr(0, host.size() - 4) + "8998";
			
		std::string sPage = LoadFile("web\\game.html");
		sPage = Util::ReplaceAll(sPage, "%PLAYER_ID%", FormatInt(player->GetID()));
		sPage = Util::ReplaceAll(sPage, "%PLAYER_NAME%", Util::ReplaceAll(player->GetName(), "'", "\'"));
		sPage = Util::ReplaceAll(sPage, "%WSURL%", wsURL);

		return CreateOKResponse(sPage);
	}
	return ""; 
}
