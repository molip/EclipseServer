#pragma once

#include <memory>

class Controller;
class Player;
class Team;
class Game;

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
	
	virtual bool CanAbort() const { return true; } 
	virtual void Undo(const Controller& controller) { } 

protected:
	Team& GetTeam() { return *m_pTeam; }
	const Team& GetTeam() const { return *m_pTeam; }
	Game& GetGame() { return *m_pGame; }
	const Game& GetGame() const { return *m_pGame; }

	Player& m_player;

private:
	Team* m_pTeam;
	Game* m_pGame;
};
