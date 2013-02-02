#include "Server.h"

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

Server::Server()
{
	//m_pageLobby = LoadFile("web\\lobby.html");
	//m_pageGame = LoadFile("web\\game.html");
}

std::string Server::GetLobbyHTML() const
{
	std::string sList;
	for (const auto& g : games)
	{
		std::ostringstream ss;
		//ss << "<a href=\"" << "/?game=" << game << "&player=\" onclick=" << GetGameURL(g.GetName(), ""); // player replaced in create_game() JS
		ss << "<a href=\"#\" onclick=\"join_game('" << g->GetName() << "')\">";
		ss << g->GetName() << " (" << g->GetOwner() << ")</a><br>" << std::endl;
		sList += ss.str();
	}
	std::string sPage = LoadFile("web\\lobby.html");
	ReplaceToken(sPage, "%GAMELIST%", sList);
	return sPage;
}
	
std::string Server::GetGameHTML(const std::string& game, const std::string& player) const
{
	std::string sPage = LoadFile("web\\game.html");
	ReplaceToken(sPage, "%GAME%", game);
	ReplaceToken(sPage, "%PLAYER%", player);
	return sPage;
}

bool Server::OnHTTPRequest(const std::string& url, const QueryMap& queries, std::string& reply)
{
	if (url == "/create")
	{
		auto i = queries.find("player");
		if (i != queries.end() && !i->second.empty())
		{
			std::ostringstream ss;
			ss << "Game" <<  games.size() + 1;
			games.push_back(GamePtr(new Game(ss.str(), i->second)));
			reply = GetRedirectHTML(GetGameURL(ss.str(), i->second));
			return true;
		}
	}
	else if (url == "/game")
	{
		if (queries.size() == 2)
		{
			auto gid = queries.find("game"), pid = queries.find("player");
			if (gid != queries.end() && pid != queries.end())
			{
				auto game = std::find_if(games.begin(), games.end(), [&] (const GamePtr& g) { return g->GetName() == gid->second; });
				if (game == games.end())
					reply = "Game not found";
				else if (!(*game)->AddPlayer(pid->second))
					reply = "Player is already playing in that game";
				else
					reply = GetGameHTML(gid->second, pid->second);
				return true;
			}
			reply = GetLobbyHTML(); // failed
			return true;
		}
	}
	else if (url == "/")
	{
		reply = GetLobbyHTML();
		return true;
	}
	return false; // WS
}

void Server::OnConnect(int clientID, const std::string& url)
{
	std::ostringstream ss;
	ss << "Client " <<  clientID << " connected";
	SendMessage(clientID, ss.str());
	std::cout << ss.str() << std::endl;
}

void Server::OnMessage(int clientID, const std::string& message)
{
	std::ostringstream ss;
	ss << "Client " <<  clientID << " said:" << message;
	SendMessage(clientID, ss.str());
	std::cout << ss.str() << std::endl;

	// TODO: register client ID for this player/game
}

void Server::OnDisconnect(int clientID) 
{
	// TODO: call RemovePlayer
	std::cout << "Client " <<  clientID << " disconnected" << std::endl;
}
