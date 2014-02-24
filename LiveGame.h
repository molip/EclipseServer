#pragma once

#include "Game.h"

class Phase;
class ActionPhase;
class ChooseTeamPhase;
class UpkeepPhase;

DEFINE_UNIQUE_PTR(Record)
DEFINE_UNIQUE_PTR(CmdStack)
DEFINE_UNIQUE_PTR(Phase)

class LiveGame : public Game
{
	friend class TurnPhase;
	friend class Phase;

public:
	enum class GamePhase { Lobby, ChooseTeam, Main };

	LiveGame();
	LiveGame(int id, const std::string& name, const Player& owner);
	virtual ~LiveGame();

	void AddPlayer(const Player& player);

	void StartChooseTeamGamePhase();
	void StartMainGamePhase();
	virtual bool HasStarted() const override { return m_gamePhase != GamePhase::Lobby; }
	virtual bool IsLive() const { return true; }

	GamePhase GetGamePhase() const { return m_gamePhase; }
	
	const Phase& GetPhase() const { return const_cast<LiveGame*>(this)->GetPhase(); };
	Phase& GetPhase();

	const ActionPhase& GetActionPhase() const { return const_cast<LiveGame*>(this)->GetActionPhase(); };
	ActionPhase& GetActionPhase();
	const ChooseTeamPhase& GetChooseTeamPhase() const { return const_cast<LiveGame*>(this)->GetChooseTeamPhase(); };
	ChooseTeamPhase& GetChooseTeamPhase();
	UpkeepPhase& GetUpkeepPhase();

	void PushRecord(std::unique_ptr<Record>& pRec);
	std::unique_ptr<Record> PopRecord();

	const std::vector<RecordPtr>& GetRecords() const { return m_records; }

	bool NeedCombat() const;

	void StartActionPhase();
	void FinishActionPhase(std::vector<Colour>& passOrder);

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	virtual void Save() const override;

	std::vector<RecordPtr> m_records;
	GamePhase m_gamePhase;
	PhasePtr m_pPhase;
};

DEFINE_UNIQUE_PTR(LiveGame)