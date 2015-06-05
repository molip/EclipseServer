#include "stdafx.h"
#include "Record.h"
#include "Team.h"
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
	RecordContext context(game2, &controller);
	Apply(true, game2, context.GetGameState());
	Update(game, context);
}

void Record::Undo(const ReviewGame& game, const Controller& controller)
{
	Game& game2 = const_cast<ReviewGame&>(game);
	RecordContext context(game2, &controller);
	Apply(false, game2, context.GetGameState());
	Update(game, context);
}

void Record::Do(LiveGame& game, const Controller* controller) 
{
	RecordContext context(game, controller);
	Apply(true, game, context.GetGameState());
	if (controller)
		Update(game, context);
}

void Record::Undo(LiveGame& game, const Controller* controller) 
{
	RecordContext context(game, controller);
	Apply(false, game, context.GetGameState());
	if (controller)
		Update(game, context);
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

void TeamRecord::Apply(bool bDo, const Game& game, GameState& gameState)
{
	Apply(bDo, game, game.GetTeam(m_colour), gameState, gameState.GetTeamState(m_colour));
}

void TeamRecord::Update(const Game& game, const RecordContext& context) const
{
	Update(game, context.GetGame().GetTeam(m_colour), context);
}
