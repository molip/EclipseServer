#pragma once

#include "Game.h"

class LiveGame;
class Controller;

DEFINE_UNIQUE_PTR(Record)

class ReviewGame : public Game
{
public:
	enum class Phase { Lobby, ChooseTeam, Main };

	ReviewGame(int id, const Player& owner, const LiveGame& live);
	virtual ~ReviewGame();

	int GetLiveGameID() const { return m_idLive; }

	virtual bool HasStarted() const override { return true; }

	void Advance(const Controller& controller);
	void Retreat(const Controller& controller);

	bool CanAdvance() const;
	bool CanRetreat() const;

	void OnPreRecordPop(const Controller& controller);

private:
	const std::vector<RecordPtr>& GetRecords() const;

	int m_idLive;
	int m_iRecord; // Last undone record.
};

DEFINE_UNIQUE_PTR(ReviewGame)