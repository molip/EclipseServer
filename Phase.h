#pragma once

#include "App.h"
#include "Dynamic.h"

class LiveGame;
class CmdStack;
class Cmd;
class Player;
class Team;

enum class Colour;

DEFINE_UNIQUE_PTR(CmdStack)
DEFINE_UNIQUE_PTR(Cmd)

class Controller;

namespace Input { class CmdMessage; }

class Phase : public Dynamic
{
public:
	Phase();
	virtual ~Phase() {}

	void SetGame(LiveGame& game) { m_pGame = &game; }
	
	void ProcessCmdMessage(const Input::CmdMessage& msg, Controller& controller, Player& player);
	void UndoCmd(Controller& controller, Player& player);
	
	virtual void AddCmd(CmdPtr pCmd) = 0;
	virtual void FinishCmd(Colour c) = 0;
	virtual Cmd* RemoveCmd(Colour c) = 0; // Returns cmd to undo.
	virtual bool CanRemoveCmd(Colour c) const = 0;
	virtual bool IsTeamActive(Colour c) const = 0;

	virtual Cmd* GetCurrentCmd(Colour c) = 0;
	const Cmd* GetCurrentCmd(Colour c) const { return const_cast<Phase*>(this)->GetCurrentCmd(c); }

	virtual void UpdateClient(const Controller& controller) const {}

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

protected:
	void SaveGame();
	const LiveGame& GetGame() const { return *m_pGame; }
	LiveGame& GetGame() { return *m_pGame; }

private:
	LiveGame* m_pGame;
};
DEFINE_UNIQUE_PTR(Phase)

class TurnPhase : public Phase
{
public:
	TurnPhase();
	virtual ~TurnPhase() {}

	const Player& GetCurrentPlayer() const;
	Player& GetCurrentPlayer();
	const Team& GetCurrentTeam() const { return const_cast<TurnPhase*>(this)->GetCurrentTeam(); }
	Team& GetCurrentTeam();

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

protected:
	void AdvanceTurn();
	int GetTurn() const { return m_iTurn; }

private:
	TurnPhase(TurnPhase& phase);

	int m_iTurn;
	int m_iStartTeam, m_iStartTeamNext;
};
