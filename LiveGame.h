#pragma once

#include "Game.h"

class Phase;
class ActionPhase;
class ChooseTeamPhase;
class UpkeepPhase;
class CombatPhase;
class ReviewGame;

DEFINE_UNIQUE_PTR(Record)
DEFINE_UNIQUE_PTR(CmdStack)
DEFINE_UNIQUE_PTR(Phase)

class LiveGame : public Game
{
	friend class TurnPhase;
	friend class Phase;

public:
	enum class GamePhase { Lobby, ChooseTeam, Main };
	typedef std::vector<std::pair<int, std::string>> LogVec;

	LiveGame();
	LiveGame(int id, const std::string& name, const Player& owner);
	virtual ~LiveGame();

	void AddPlayer(const Player& player);
	void AssignTeam(Player& player, RaceType race, Colour colour);

	void StartChooseTeamGamePhase();
	void StartMainGamePhase();
	virtual bool HasStarted() const override { return m_gamePhase != GamePhase::Lobby; }
	virtual bool IsLive() const override { return true; }
	virtual LogVec GetLogs() const override;

	GamePhase GetGamePhase() const { return m_gamePhase; }
	
	const Phase& GetPhase() const { return const_cast<LiveGame*>(this)->GetPhase(); };
	Phase& GetPhase();

	const ActionPhase& GetActionPhase() const { return const_cast<LiveGame*>(this)->GetActionPhase(); };
	ActionPhase& GetActionPhase();
	const ChooseTeamPhase& GetChooseTeamPhase() const { return const_cast<LiveGame*>(this)->GetChooseTeamPhase(); };
	ChooseTeamPhase& GetChooseTeamPhase();
	UpkeepPhase& GetUpkeepPhase();
	const CombatPhase& GetCombatPhase() const;

	int PushRecord(RecordPtr pRec); // Returns record id.
	RecordPtr PopRecord();

	const std::vector<RecordPtr>& GetRecords() const { return m_records; }
	int GetLastPoppableRecord() const; 

	const std::set<ReviewGame*> GetReviewGames() const { return m_reviewGames; }
	void AddReviewGame(ReviewGame& game) const { m_reviewGames.insert(&game); }
	void RemoveReviewGame(ReviewGame& game) const { m_reviewGames.erase(&game); }

	bool NeedCombat() const;

	void StartActionPhase();
	void FinishActionPhase(const std::vector<Colour>& passOrder);
	void FinishCombatPhase();

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	void Save() const;
	std::mutex& GetMutex() const { return m_mutex; }

private:
	std::vector<RecordPtr> m_records;
	GamePhase m_gamePhase;
	PhasePtr m_pPhase;
	int m_nextRecordID;

	// Not saved.
	mutable std::mutex m_mutex;
	mutable std::set<ReviewGame*> m_reviewGames;
};

DEFINE_UNIQUE_PTR(LiveGame)