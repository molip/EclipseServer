#include "stdafx.h"
#include "ReviewGame.h"
#include "LiveGame.h"
#include "Games.h"
#include "App.h"
#include "Player.h"
#include "Record.h"


ReviewGame::ReviewGame(int id, const Player& owner, const LiveGame& live) : 
	Game(id, live.GetName() + " [review]", owner, live), m_idLive(live.GetID())
{
	m_iRecord = live.GetRecords().size();
}

ReviewGame::~ReviewGame()
{
}

const std::vector<RecordPtr>& ReviewGame::GetRecords() const
{
	return Games::GetLive(m_idLive).GetRecords();
}

void ReviewGame::Advance(const Controller& controller)
{
	AssertThrow("ReviewGame::Advance", CanAdvance());
	GetRecords()[m_iRecord++]->Do(*this, controller);
}

void ReviewGame::Retreat(const Controller& controller)
{
	AssertThrow("ReviewGame::Retreat", CanRetreat());
	GetRecords()[--m_iRecord]->Undo(*this, controller);
}

bool ReviewGame::CanAdvance() const
{
	return m_iRecord < (int)GetRecords().size();
}

bool ReviewGame::CanRetreat() const
{
	return m_iRecord > 0;
}

void ReviewGame::OnPreRecordPop(const Controller& controller)
{
	AssertThrow("ReviewGame::OnPreRecordPop", m_iRecord > 0 && m_iRecord <= (int)GetRecords().size());
	if (m_iRecord == GetRecords().size())
		Retreat(controller);
}
