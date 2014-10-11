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

std::vector<std::pair<int, std::string>> ReviewGame::GetLogs() const
{
	return Games::GetLive(m_idLive).GetLogs();
}

void ReviewGame::Advance(const Controller& controller)
{
	VERIFY_MODEL_MSG("Already at end", CanAdvance());
	Record& rec = *GetRecords()[m_iRecord++];
	rec.Do(*this, controller);

	// Skip messages.
	while (CanAdvance() && GetRecords()[m_iRecord]->IsMessageRecord())
		++m_iRecord;
}

void ReviewGame::Retreat(const Controller& controller)
{
	VERIFY_MODEL_MSG("Already at start", CanRetreat());

	// Skip messages.
	while (CanRetreat() && GetRecords()[m_iRecord - 1]->IsMessageRecord())
		--m_iRecord;

	if (CanRetreat())
		GetRecords()[--m_iRecord]->Undo(*this, controller);
}

bool ReviewGame::CanAdvance() const
{
	return m_iRecord < (int)GetRecords().size();
}

bool ReviewGame::CanRetreat() const
{
	return m_iRecord > 1;
}

int ReviewGame::GetNextRecordID() const
{
	return CanAdvance() ? GetRecords()[m_iRecord]->GetID() : 0;
}

void ReviewGame::OnPreRecordPop(const Controller& controller)
{
	int pop = Games::GetLive(m_idLive).GetLastPoppableRecord();
	VERIFY_MODEL(CanRetreat() && m_iRecord <= (int)GetRecords().size() && pop >= 0);

	if (m_iRecord > pop)
		GetRecords()[pop]->Undo(*this, controller);

	if (m_iRecord >= pop) // Skip trailing messages. 
		m_iRecord = GetRecords().size() - 1;
}
