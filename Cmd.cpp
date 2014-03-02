#include "stdafx.h"
#include "Cmd.h"
#include "Player.h"
#include "Team.h"
#include "LiveGame.h"
#include "Record.h"
#include "CommitSession.h"

Cmd::Cmd() : m_colour(Colour::None), m_bHasRecord(false) {}

Cmd::Cmd(Colour colour) : m_colour(colour), m_bHasRecord(false)
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

void Cmd::DoRecord(RecordPtr pRec, CommitSession& session)
{
	VerifyModel("Cmd::DoRecord", !m_bHasRecord);

	session.DoAndPushRecord(std::move(pRec));
	m_bHasRecord = true;
}

void Cmd::PopRecord(CommitSession& session)
{
	VerifyModel("Cmd::PopRecord", m_bHasRecord);
	session.PopAndUndoRecord();
	m_bHasRecord = false;
}

void Cmd::Save(Serial::SaveNode& node) const 
{
	node.SaveEnum("colour", m_colour);
	node.SaveType("has_record", m_bHasRecord);
}

void Cmd::Load(const Serial::LoadNode& node) 
{
	node.LoadEnum("colour", m_colour);
	node.LoadType("has_record", m_bHasRecord);
}
