#include "CommitSession.h"
#include "Record.h"
#include "LiveGame.h"
#include "SaveThread.h"
#include "Controller.h"
#include "Output.h"
#include "ReviewGame.h"

// All non-const LiveGame access should be performed during a CommitSession. 
// If an exception occurs between opening and committing the session, the game may be in an invalid state and should be locked. 
// Nesting is not allowed. 

CommitSession* CommitSession::s_pInstance;

CommitSession::CommitSession(const LiveGame& game, const Controller& controller) : 
m_game(const_cast<LiveGame&>(game)), m_controller(controller), m_bCommitted(), m_lock(game.GetMutex(), std::defer_lock),
m_bUpdateReviewUI(false)
{
	VERIFY(!s_pInstance);
	s_pInstance = this;
}

CommitSession::~CommitSession()
{
	s_pInstance = nullptr;

	//if (m_lock && !m_bCommitted)
	//	TODO: Lock game.
}

LiveGame& CommitSession::Open() 
{
	if (!m_lock)
		if (!m_lock.try_lock())
		{
			std::cout << "Waiting for game mutex..." << std::endl;
			m_lock.lock();
		}
	return m_game; 
}

void CommitSession::Commit()
{
	VERIFY(!m_bCommitted);
	m_bCommitted = true;

	if (m_lock)
	{
		m_lock.unlock();
		SaveThread::Instance()->Push(m_game);
	}

	if (m_bUpdateReviewUI)
		for (auto& g : m_game.GetReviewGames())
			m_controller.SendMessage(Output::UpdateReviewUI(*g), *g);
}

void CommitSession::DoAndPushRecord(RecordPtr pRec)
{
	Open();
	pRec->Do(m_game, m_controller);

	m_bUpdateReviewUI |= !pRec->IsMessageRecord();

	std::string msg = pRec->GetMessage(m_game);

	int id = m_game.PushRecord(std::move(pRec));

	ASSERT(!msg.empty());

	Output::AddLog output(id, msg);
	m_controller.SendMessage(output, m_game);
	
	for (auto& g : m_game.GetReviewGames())
		m_controller.SendMessage(output, *g);
}

RecordPtr CommitSession::PopAndUndoRecord()
{
	Open();
	RecordPtr pRec = m_game.PopRecord();
	pRec->Undo(m_game, m_controller);

	m_bUpdateReviewUI |= !pRec->IsMessageRecord();

	Output::RemoveLog output(pRec->GetID());
	m_controller.SendMessage(output, m_game);

	for (auto& g : m_game.GetReviewGames())
		m_controller.SendMessage(output, *g);

	return pRec;
}
