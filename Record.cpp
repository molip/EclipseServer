#include "stdafx.h"
#include "Record.h"
#include "Team.h"
#include "Serial.h"
#include "LiveGame.h"
#include "ReviewGame.h"
#include "Controller.h"
#include "Output.h"

Record::Record() {}

Record::~Record() {}

RecordPtr Record::PopAndUndo(const LiveGame& game, const Controller& controller)
{
	LiveGame& game2 = const_cast<LiveGame&>(game);
	RecordPtr pRec = game2.PopRecord();
	pRec->Apply(false, game2, controller);
	return pRec;
}

void Record::DoAndPush(RecordPtr pRec, const LiveGame& game, const Controller& controller)
{
	LiveGame& game2 = const_cast<LiveGame&>(game);
	pRec->Apply(true, game2, controller);
	game2.PushRecord(pRec);
}

void Record::Do(RecordPtr pRec, const LiveGame& game, const Controller& controller)
{
	pRec->Apply(true, const_cast<LiveGame&>(game), controller);
}

void Record::DoImmediate(const LiveGame& game, const std::function<void(LiveGame&)>& fn)
{
	fn(const_cast<LiveGame&>(game));
}

void Record::DoImmediate(const ReviewGame& game, const std::function<void(ReviewGame&)>& fn)
{
	fn(const_cast<ReviewGame&>(game));
}

void Record::Do(const ReviewGame& game, const Controller& controller)
{
	Game& game2 = const_cast<ReviewGame&>(game);
	Apply(true, game2, controller);
}

void Record::Undo(const ReviewGame& game, const Controller& controller)
{
	Game& game2 = const_cast<ReviewGame&>(game);
	Apply(false, game2, controller);
}

//-----------------------------------------------------------------------------

TeamRecord::TeamRecord() : m_colour(Colour::None) {}
TeamRecord::TeamRecord(Colour colour) : m_colour(colour) {}

void TeamRecord::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveEnum("colour", m_colour);
}
	
void TeamRecord::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadEnum("colour", m_colour);
}
