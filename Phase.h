#pragma once

#include "App.h"
#include "Dynamic.h"

class LiveGame;
class CmdStack;
class Cmd;
class Player;
class Team;
class CommitSession;

enum class Colour;

DEFINE_UNIQUE_PTR(CmdStack)
DEFINE_UNIQUE_PTR(Cmd)

class Controller;

namespace Input { class CmdMessage; }

class Phase : public Dynamic
{
public:
	Phase(LiveGame* pGame);
	virtual ~Phase() {}

	void SetGame(LiveGame& game) { m_pGame = &game; }
	
	void ProcessCmdMessage(const Input::CmdMessage& msg, CommitSession& session, const Player& player);
	void UndoCmd(CommitSession& session, Player& player);
	
	virtual void Init(CommitSession& session) {}

	virtual void StartCmd(CmdPtr pCmd, CommitSession& session);

	virtual bool CanRemoveCmd(Colour c) const;

	virtual Cmd* GetCurrentCmd(Colour c);
	const Cmd* GetCurrentCmd(Colour c) const { return const_cast<Phase*>(this)->GetCurrentCmd(c); }

	virtual void UpdateClient(const Controller& controller, const Player* pPlayer) const {}

	virtual std::vector<const Team*> GetCurrentTeams() const { return{}; }

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

protected:
	const CmdStack& GetCmdStack(Colour c) const { return const_cast<Phase*>(this)->GetCmdStack(c); }

	virtual CmdStack& GetCmdStack(Colour c) = 0;
	virtual void OnCmdFinished(const Cmd& cmd, CommitSession& session) {}
	virtual Cmd* RemoveCmd(CommitSession& session, Colour c);

	const LiveGame& GetGame() const { return *m_pGame; }
	LiveGame& GetGame() { return *m_pGame; }

private:
	LiveGame* m_pGame;
};
DEFINE_UNIQUE_PTR(Phase)

class TurnPhase
{
public:
	TurnPhase();
	virtual ~TurnPhase() {}

	const Team& GetCurrentTeam(const LiveGame& game) const;

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

protected:
	void AdvanceTurn();
	int GetTurn() const { return m_iTurn; }

private:
	int m_iTurn;
};
