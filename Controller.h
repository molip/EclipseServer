#pragma once

#include "Input.h"

#include "libKernel/Singleton.h"

#include <string>
#include <vector>
#include <memory>

class WSServer;
class Game;

namespace Output { class Message; }

class Controller : public Singleton<Controller>
{
	friend class WSServer;
public:
	Controller();
	void SetServer(WSServer* p) { m_pServer = p; }

	void OnMessage(const Input::MessagePtr& pMsg, Player& player);
	void OnPlayerConnected(Player& player);
	void OnPlayerDisconnected(Player& player);

	void SendMessage(const Output::Message& msg, const Player& player) const;
	void SendMessage(const Output::Message& msg, const Game& game, const Player* pPlayer = nullptr) const;
	
	void SendUpdateGameList(const Player* pPlayer = nullptr) const;
	void SendUpdateGame(const Game& game, const Player* pPlayer = nullptr) const;

private:
	typedef std::shared_ptr<std::string> StringPtr;

	void SendMessage(StringPtr msg, const Player& player) const;
	void SendQueuedMessages();
	void ClearQueuedMessages();

	WSServer* m_pServer;
	mutable std::map<const Player*, std::vector<StringPtr>> m_messages;
};
