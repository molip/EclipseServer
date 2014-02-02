#include "stdafx.h"
#include "ChooseTeamPhase.h"
#include "LiveGame.h"

ChooseTeamPhase::ChooseTeamPhase()
{
}

bool ChooseTeamPhase::IsTeamActive(Colour c) const 
{
	return c == GetCurrentTeam().GetColour();
}

void ChooseTeamPhase::AssignTeam(Player& player, RaceType race, Colour colour)
{
	GetGame().GetTeam(player).Assign(race, colour);

	AdvanceTurn();
	SaveGame();

	if (GetTurn() == 0)
		GetGame().StartMainGamePhase();
}

REGISTER_DYNAMIC(ChooseTeamPhase)

