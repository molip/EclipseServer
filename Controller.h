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

	void OnMessage(const Input::MessagePtr& pMsg, Player& player);
	void OnPlayerConnected(Player& player);
	void OnPlayerDisconnected(Player& player);

	bool SendMessage(const Output::Message& msg, const Player& player) const;
	bool SendMessage(const Output::Message& msg, const Game& game, const Player* pPlayer = nullptr) const;

	Model& GetModel() { return m_model; }
	
	void SendUpdateGameList(const Player* pPlayer = nullptr) const;
	void SendUpdateGame(const Game& game, const Player* pPlayer = nullptr) const;

private:
	Model& m_model;
	WSServer* m_pServer;
};
