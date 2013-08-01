#pragma once

#include "Game.h"

DEFINE_UNIQUE_PTR(Record)

class LiveGame : public Game
{
public:
	enum class Phase { Lobby, ChooseTeam, Main };

	LiveGame();
	LiveGame(int id, const std::string& name, const Player& owner);
	virtual ~LiveGame();

	void AddPlayer(Player& player);
	void AssignTeam(Player& player, RaceType race, Colour colour);

	void StartChooseTeamPhase();
	void StartMainPhase();
	virtual bool HasStarted() const override { return m_phase != Phase::Lobby; }
	virtual bool IsLive() const { return true; }
	Phase GetPhase() const { return m_phase; }

	void StartCmd(CmdPtr pCmd);
	void AddCmd(CmdPtr pCmd);
	Cmd* RemoveCmd(); // Returns cmd to undo.
	bool PurgeCmds();
	bool CanRemoveCmd() const;
	bool CanDoAction() const;

	Cmd* GetCurrentCmd();
	const Cmd* GetCurrentCmd() const;

	void PushRecord(std::unique_ptr<Record>& pRec);
	std::unique_ptr<Record> PopRecord();
	const std::vector<RecordPtr>& GetRecords() const { return m_records; }
	
	const Player& GetCurrentPlayer() const;
	Player& GetCurrentPlayer();
	const Team& GetCurrentTeam() const	{ return const_cast<LiveGame*>(this)->GetCurrentTeam(); }
	Team& GetCurrentTeam();

	void FinishTurn();
	void StartRound();

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	virtual void Save() const override;

	void AdvanceTurn();

	CmdStack* m_pCmdStack;
	std::vector<RecordPtr> m_records;
	Phase m_phase;
	bool m_bDoneAction;

	int m_iTurn, m_iRound;
	int m_iStartTeam, m_iStartTeamNext;
};

DEFINE_UNIQUE_PTR(LiveGame)