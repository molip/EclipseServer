#pragma once

#include "Game.h"

class LiveGame : public Game
{
public:
	enum class Phase { Lobby, ChooseTeam, Main };

	LiveGame(int id, const std::string& name, Player& owner);
	virtual ~LiveGame();

	void AddPlayer(Player& player);
	void AssignTeam(Player& player, RaceType race, Colour colour);

	void StartChooseTeamPhase();
	void StartMainPhase();
	virtual bool HasStarted() const override { return m_phase != Phase::Lobby; }
	Phase GetPhase() const { return m_phase; }

	void StartCmd(CmdPtr pCmd);
	void AddCmd(CmdPtr pCmd);
	Cmd* RemoveCmd(); // Returns cmd to undo.
	bool CanRemoveCmd() const;
	bool CanDoAction() const;

	Cmd* GetCurrentCmd();
	const Cmd* GetCurrentCmd() const;

	void PushRecord(std::unique_ptr<Record>& pRec);
	std::unique_ptr<Record> PopRecord();

	virtual void FinishTurn() override;

private:
	CmdStack* m_pCmdStack;
	std::list<std::unique_ptr<Record>> m_records;
	Phase m_phase;
};

DEFINE_UNIQUE_PTR(LiveGame)