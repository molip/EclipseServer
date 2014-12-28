#include "stdafx.h"
#include "Record.h"
#include "Team.h"
#include "Serial.h"
#include "LiveGame.h"
#include "ReviewGame.h"
#include "Controller.h"
#include "Output.h"
#include "Player.h"

RecordContext::RecordContext(Game& game, const Controller* controller) : m_game(game), m_controller(controller) 
{
}

void RecordContext::SendMessage(const Output::Message& msg, const Player* pPlayer) const
{
	if (m_controller)
		m_controller->SendMessage(msg, m_game, pPlayer);
}

//-----------------------------------------------------------------------------

Record::Record() : m_id(-1) {}

Record::~Record() {}

void Record::DoImmediate(const ReviewGame& game, const std::function<void(ReviewGame&)>& fn)
{
	fn(const_cast<ReviewGame&>(game));
}

void Record::Do(const ReviewGame& game, const Controller& controller)
{
	Game& game2 = const_cast<ReviewGame&>(game);
	Apply(true, RecordContext(game2, &controller));
}

void Record::Undo(const ReviewGame& game, const Controller& controller)
{
	Game& game2 = const_cast<ReviewGame&>(game);
	Apply(false, RecordContext(game2, &controller));
}

void Record::Do(LiveGame& game, const Controller* controller) 
{
	Apply(true, RecordContext(game, controller));
}

void Record::Undo(LiveGame& game, const Controller* controller) 
{
	Apply(false, RecordContext(game, controller));
}

void Record::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveType("id", m_id);
}

void Record::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadType("id", m_id);
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

std::string TeamRecord::GetMessage(const Game& game) const
{ 
	std::string msg = GetTeamMessage();
	if (!msg.empty())
	{
		std::string player = game.GetTeam(m_colour).GetPlayer().GetName();
		return FormatString("%0 (%1) %2", player, ::EnumToString(m_colour), msg);
	}
	VERIFY(false);
	return "";
}

void TeamRecord::Apply(bool bDo, const RecordContext& context)
{
	Apply(bDo, context.GetGame().GetTeam(m_colour), context.GetGameState().GetTeamState(m_colour), context);
}
