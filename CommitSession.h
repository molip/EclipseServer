#pragma once

#include "App.h"

#include <memory>
#include <vector>
#include <functional>

class LiveGame;
class Controller;

class Record;
DEFINE_UNIQUE_PTR(Record);

class CommitSession
{
public:
	CommitSession(const LiveGame& game, const Controller& controller);
	~CommitSession();

	const LiveGame& GetGame() const { return m_game; }
	LiveGame& Open();
	const Controller& GetController() const { return m_controller; }

	void DoAndPushRecord(RecordPtr pRec);
	RecordPtr PopAndUndoRecord();

	void Commit();

private:
	LiveGame& m_game;
	const Controller& m_controller;
	static CommitSession* s_pInstance;
	bool m_bOpened, m_bCommitted, m_bUpdateReviewUI;
	std::unique_lock<std::mutex> m_lock;
};
