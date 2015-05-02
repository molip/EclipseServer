#include "stdafx.h"
#include "StartGameRecord.h"
#include "Output.h"
#include "Controller.h"
#include "Player.h"
#include "LiveGame.h"

StartGameRecord::StartGameRecord()
{
}

void StartGameRecord::Apply(bool bDo, const Game& game, GameState& gameState) 
{
	gameState.IncrementRound(bDo);

	const int startTech[] = { 12, 12, 14, 16, 18, 20 };

	gameState.ProduceTechnologies(startTech[game.GetTeams().size() - 1], bDo);
}

void StartGameRecord::Update(const Game& game, const RecordContext& context) const
{
	context.SendMessage(Output::UpdateRound(game));
	context.SendMessage(Output::UpdateTechnologies(game));
}

std::string StartGameRecord::GetMessage(const Game& game) const
{
	return FormatString("Starting round 1");
}

REGISTER_DYNAMIC(StartGameRecord)

