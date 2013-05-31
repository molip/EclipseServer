#pragma once

#include <memory>

class Controller;
class Player;
class Team;
class LiveGame;

namespace Input { class CmdMessage; }

class Cmd;
typedef std::unique_ptr<Cmd> CmdPtr;

class Cmd
{
public:
	Cmd(Player& player);
	virtual ~Cmd() {}
	
	virtual void UpdateClient(const Controller& controller) const {}
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller) = 0;
	
	virtual bool IsAction() const { return false; } 
	virtual bool CanUndo() const { return true; } 
	virtual void Undo(const Controller& controller) { } 

protected:
	Team& GetTeam() { return *m_pTeam; }
	const Team& GetTeam() const { return *m_pTeam; }
	LiveGame& GetGame() { return *m_pGame; }
	const LiveGame& GetGame() const { return *m_pGame; }

	Player& m_player;

private:
	Team* m_pTeam;
	LiveGame* m_pGame;
};
