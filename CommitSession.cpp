#include "CommitSession.h"
#include "Record.h"
#include "LiveGame.h"
#include "SaveThread.h"

// All non-const LiveGame access should be performed during a CommitSession. 
// If an exception occurs between opening and committing the session, the game may be in an invalid state and should be locked. 
// Nesting is not allowed. 

CommitSession* CommitSession::s_pInstance;

CommitSession::CommitSession(const LiveGame& game, const Controller& controller) : 
m_game(const_cast<LiveGame&>(game)), m_controller(controller), m_bCommitted(), m_lock(game.GetMutex(), std::defer_lock)
{
	Verify("CommitSession::CommitSession", !s_pInstance);
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
	Verify("CommitSession::Commit", !m_bCommitted);
	m_bCommitted = true;

	if (m_lock)
	{
		m_lock.unlock();
		SaveThread::Instance()->Push(m_game);
	}
}

void CommitSession::DoAndPushRecord(RecordPtr pRec)
{
	Open();
	pRec->Do(m_game, m_controller);
	m_game.PushRecord(pRec);
}

RecordPtr CommitSession::PopAndUndoRecord()
{
	Open();
	RecordPtr pRec = m_game.PopRecord();
	pRec->Undo(m_game, m_controller);
	return pRec;
}

void CommitSession::DoRecord(RecordPtr pRec)
{
	Open();
	pRec->Do(m_game, m_controller);
}

void CommitSession::DoSession(const LiveGame& game, const Controller& controller, const std::function<void(CommitSession&)>& fn)
{
	CommitSession session(game, controller);
	fn(session);
	session.Commit();
}
