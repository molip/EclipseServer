#pragma once

#include "Singleton.h"
#include "Input.h"

#include <string>
#include <vector>

class Model;
class WSServer;
class Game;

namespace Output { class Message; }

class Controller : public Singleton<Controller>
{
public:
	Controller(Model& model);
	void SetServer(WSServer* p) { m_pServer = p; }

	void OnMessage(const Input::MessagePtr& pMsg, const std::string& player);
	void OnPlayerConnected(const std::string& player);
	void OnPlayerDisconnected(const std::string& player);

	bool SendMessage(const Output::Message& msg, const std::string& player = "") const;
	bool SendMessage(const Output::Message& msg, const Game& game) const;

	Model& GetModel() { return m_model; }
	
	void SendUpdateGameList(const std::string& player = "") const;

	//Player* FindPlayer(const std::string& name) { return m_players.FindPlayer(name); }
	//const Player* FindPlayer(const std::string& name) const { return m_players.FindPlayer(name); }

	const Game* GetPlayerGame(const std::string& player) const;
	void SetPlayerGame(const std::string& player, const Game* pGame);

private:
	void UnregisterPlayerFromGame(const std::string& player, const Game* pGame);

	std::map<std::string, const Game*>		m_players;
	std::map<const Game*, std::set<std::string>>	m_games;

	Model& m_model;
	WSServer* m_pServer;
};
