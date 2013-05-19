#include "stdafx.h"
#include "Cmd.h"
#include "Player.h"
#include "Team.h"
#include "Game.h"

Cmd::Cmd(Player& player) : m_player(player)
{
	m_pGame = player.GetCurrentGame();
	m_pTeam = player.GetCurrentTeam();

	AssertThrow("Cmd::Cmd", m_pGame && m_pTeam);
}



