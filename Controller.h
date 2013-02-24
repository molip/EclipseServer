#pragma once

#include "Singleton.h"
#include "Input.h"
#include "Player.h"

#include <string>
#include <vector>

class Model;
class WSServer;

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
	
	void UpdateGameList(const std::string& player = "") const;

	Player* FindPlayer(const std::string& name) { return m_players.FindPlayer(name); }
	const Player* FindPlayer(const std::string& name) const { return m_players.FindPlayer(name); }

private:
	Model& m_model;
	WSServer* m_pServer;
	Players m_players;
};
