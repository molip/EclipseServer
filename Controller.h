#pragma once

#include <string>
#include <vector>

class Model;
class WSServer;

class Controller
{
public:
	Controller(Model& model);
	void SetServer(WSServer* p) { m_pServer = p; }

	void OnMessage(const std::string& player, const std::string& msg);
	void OnPlayerRegistered(const std::string& player);

private:
	void UpdateGameList(const std::string& player = "") const;

	Model& m_model;
	WSServer* m_pServer;
};
