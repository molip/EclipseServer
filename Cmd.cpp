#include "stdafx.h"
#include "Cmd.h"
#include "Player.h"
#include "Team.h"
#include "LiveGame.h"

Cmd::Cmd() : m_colour(Colour::None) {}

Cmd::Cmd(Colour colour) : m_colour(colour)
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

void Cmd::Save(Serial::SaveNode& node) const 
{
	node.SaveEnum("colour", m_colour);
}

void Cmd::Load(const Serial::LoadNode& node) 
{
	node.LoadEnum("colour", m_colour);
}
