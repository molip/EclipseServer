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

void Record::Do(LiveGame& game, const Controller& controller) 
{
	Apply(true, game, controller); 
}

void Record::Undo(LiveGame& game, const Controller& controller) 
{
	Apply(false, game, controller); 
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

std::string TeamRecord::GetMessage(const Game& game, bool bUndo) const
{ 
	std::string msg = GetTeamMessage(bUndo);
	if (!msg.empty())
		return FormatString("%0 %1: %2", EnumTraits<Colour>::ToString(m_colour), bUndo ? "undid" : "did", msg);

	return __super::GetMessage(game, bUndo);
}
