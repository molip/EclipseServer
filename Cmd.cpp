#include "stdafx.h"
#include "Cmd.h"
#include "Player.h"
#include "Team.h"
#include "LiveGame.h"
#include "Record.h"
#include "CommitSession.h"

Cmd::Cmd() : m_colour(Colour::None), m_recordCount(0) {}

Cmd::Cmd(Colour colour) : m_colour(colour), m_recordCount(0)
{
}

Team& Cmd::GetTeam(LiveGame& game) 
{
	return game.GetTeam(m_colour); 
}

const Team& Cmd::GetTeam(const LiveGame& game) const 
{
	return game.GetTeam(m_colour); 
}

Player& Cmd::GetPlayer(LiveGame& game)
{
	return GetTeam(game).GetPlayer(); 
}

const Player& Cmd::GetPlayer(const LiveGame& game) const
{
	return GetTeam(game).GetPlayer(); 
}

std::string Cmd::GetActionName() const
{
	VERIFY(false);
	return "";
}

void Cmd::DoRecord(RecordPtr pRec, CommitSession& session)
{
	session.DoAndPushRecord(std::move(pRec));
	++m_recordCount;
}

void Cmd::PopRecord(CommitSession& session)
{
	VERIFY_MODEL(m_recordCount > 0 && CanUndo());
	session.PopAndUndoRecord();
	--m_recordCount;
}

void Cmd::Save(Serial::SaveNode& node) const 
{
	node.SaveEnum("colour", m_colour);
	node.SaveType("record_count", m_recordCount);
}

void Cmd::Load(const Serial::LoadNode& node) 
{
	node.LoadEnum("colour", m_colour);
	node.LoadType("record_count", m_recordCount);
}
