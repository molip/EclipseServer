#pragma once

#include "Singleton.h"
#include "Input.h"

#include <string>
#include <vector>

class Model;
class WSServer;

class Controller : public Singleton<Controller>
{
public:
	Controller(Model& model);
	void SetServer(WSServer* p) { m_pServer = p; }

	void OnMessage(const Input::MessagePtr& pMsg, const std::string& player);
	void OnPlayerRegistered(const std::string& player);

	Model& GetModel() { return m_model; }
	
	void UpdateGameList(const std::string& player = "") const;

private:

	Model& m_model;
	WSServer* m_pServer;
};
