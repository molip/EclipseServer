#include "stdafx.h"
#include "HTMLServer.h"

#include <fstream>

#include "App.h"
#include "Game.h"
#include "Controller.h"
#include "Players.h"
#include "Invitations.h"
#include "Util.h"
#include "PlayerList.h"

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

Player* HTMLServer::Authenticate(const std::string& email, const std::string& password)
{
	Player* pPlayer = Players::Find(email);
	return pPlayer && pPlayer->CheckPassword(password) ? pPlayer : nullptr;
}

bool HTMLServer::Authenticate(int playerId, const StringMap& cookies)
{
	if (const Player* player = Players::Find(playerId))
	{
		PlayerList players(playerId, cookies);
		return players.GetCurrent() != nullptr;
	}
	return false;
}

std::string HTMLServer::OnHTTPRequest(const std::string& url, const std::string& host, const Request& request)
{
	PlayerList players(request);

	if (url == "/login")
	{
		auto postData = request.GetPostData();

		auto email = postData.Get("email");
		auto password = postData.Get("password");

		if (Player* player = Authenticate(email, password))
		{
			players.Add(*player);
			return players.GetResponse();
		}

		return CreateRedirectResponse("/login.html?failed=1");
	}

	if (url == "/register")
	{
		auto postData = request.GetPostData();

		auto email = postData.Get("email");
		auto name = postData.Get("name");
		auto password = postData.Get("password1");
		auto code = postData.Get("code");

		auto redirect = [&](const std::string& error)
		{
			const std::string& errorParam = error.empty() ? "" : ::FormatString("&error=%0", error);
			return CreateRedirectResponse(::FormatString("/register.html?code=%0%1", code, errorParam));
		};

		if (email.empty() || name.empty() || password.empty()) // Shouldn't happen, checked client-side.
			return redirect("");
		
		if (Players::Find(email))
			return redirect("email_taken");

		if (!Invitations::Find(code))
			return redirect("invalid_code");

		Player& player = Players::Add(email, name, password);
		Invitations::Remove(code);

		players.Add(player);
		return players.GetResponse();
	}

	// Login & register don't need a player. Other resources do, but any player will do. 
	if (players.GetPlayers().empty() && url != "/login.html" && url != "/register.html" && url != "/util.js")
		return CreateRedirectResponse("/login.html");

	if (url == "/logout")
	{
		players.RemoveCurrent();
		return players.GetResponse("/login.html");
	}

	if (url == "/login.html")
	{
		std::string sPage = LoadFile("web\\login.html");

		std::string js;
		for (auto* player : players.GetPlayers())
			js += ::FormatString("{ id:%0, name:'%1' }, ", player->GetID(), Util::ReplaceAll(player->GetName(), "'", "\\'"));
		sPage = Util::ReplaceAll(sPage, "%PLAYER_LIST%", js);

		return CreateOKResponse(sPage);
	}

	if (url == "/")
	{
		const Player* player = players.GetCurrent();
		if (!player)
			return CreateRedirectResponse("/login.html");

		ASSERT(host.substr(host.size() - 5) == ":8999");
		std::string wsURL = GetWebSocketScheme() + "://" + host.substr(0, host.size() - 4) + "8998";
			
		std::string sPage = LoadFile("web\\game.html");
		sPage = Util::ReplaceAll(sPage, "%PLAYER_ID%", FormatInt(player->GetID()));
		sPage = Util::ReplaceAll(sPage, "%PLAYER_NAME%", Util::ReplaceAll(player->GetName(), "'", "\'"));
		sPage = Util::ReplaceAll(sPage, "%WSURL%", wsURL);

		return CreateOKResponse(sPage);
	}

	return ""; // Default request handling. 
}
