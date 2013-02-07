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

	void UpdateGameList(const std::string& player = "") const;
	void OnCommand(const std::string& player, const std::string& cmd, const std::vector<std::string>& params);

private:
	Model& m_model;
	WSServer* m_pServer;
};
