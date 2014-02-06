#include "stdafx.h"
#include "ChooseTeamPhase.h"
#include "LiveGame.h"
#include "Output.h"
#include "Controller.h"
#include "StartRoundRecord.h"

ChooseTeamPhase::ChooseTeamPhase(LiveGame* pGame) : TurnPhase(pGame)
{
}

bool ChooseTeamPhase::IsTeamActive(Colour c) const 
{
	return c == GetCurrentTeam().GetColour();
}

void ChooseTeamPhase::AssignTeam(Controller& controller, Player& player, RaceType race, Colour colour)
{
	LiveGame& game = GetGame();
	
	game.GetTeam(player).Assign(race, colour);

	AdvanceTurn();

	controller.SendMessage(Output::ChooseTeam(game, false), player);

	bool allAssigned = true;
	for (auto& team : game.GetTeams())
		allAssigned &= team->GetColour() != Colour::None;
	
	if (allAssigned)
	{
		StartRoundRecord().Do(game, controller);
		game.StartMainGamePhase(); // Deletes this.
	}

	controller.SendUpdateGame(game);
}

void ChooseTeamPhase::UpdateClient(const Controller& controller, const Player* pPlayer) const
{
	controller.SendMessage(Output::ShowChoose(), GetGame(), pPlayer);
	controller.SendMessage(Output::UpdateChoose(GetGame()), GetGame(), pPlayer);

	if (!pPlayer || pPlayer == &GetCurrentPlayer())
		controller.SendMessage(Output::ChooseTeam(GetGame(), true), GetCurrentPlayer());

	return;
}

REGISTER_DYNAMIC(ChooseTeamPhase)

