#include "stdafx.h"
#include "Cmd.h"
#include "Player.h"
#include "Team.h"
#include "LiveGame.h"
#include "Record.h"

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

void Cmd::DoRecord(RecordPtr pRec, const Controller& controller, const LiveGame& game)
{
	VerifyModel("Cmd::DoRecord", !m_bHasRecord);

	Record::DoAndPush(std::move(pRec), game, controller);
	m_bHasRecord = true;
}

void Cmd::PopRecord(const Controller& controller, const LiveGame& game)
{
	VerifyModel("Cmd::PopRecord", m_bHasRecord);
	Record::PopAndUndo(game, controller);
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
