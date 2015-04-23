#include "Test.h"
#include "App.h"
#include "Hex.h"
#include "Map.h"
#include "LiveGame.h"
#include "Games.h"
#include "Players.h"
#include "Controller.h"
#include "CommitSession.h"
#include "ChooseTeamPhase.h"
#include "UpkeepPhase.h"
#include "ActionPhase.h"
#include "CombatPhase.h"
#include "Dice.h"
#include "AttackRecord.h"
#include "ShipBattle.h"

void Test::Run()
{
	Player& player1 = Players::AddTest();
	Player& player2 = Players::AddTest();
	LiveGame& game = Games::AddTest(player1);
	player1.SetCurrentGame(&game);
	player2.SetCurrentGame(&game);

	game.EnjoinPlayer(player1, true);
	game.EnjoinPlayer(player2, true);
	
	game.StartChooseTeamGamePhase();

	Controller controller;
	CommitSession session(game, controller);

	game.GetChooseTeamPhase().AssignTeam(session, player1, RaceType::Human, Colour::Red);
	game.GetChooseTeamPhase().AssignTeam(session, player2, RaceType::Human, Colour::Blue);

	GameState& gameState = GetGameState(game);
	TeamState& teamstate1 = gameState.GetTeamState(Colour::Red);
	TeamState& teamstate2 = gameState.GetTeamState(Colour::Blue);

	Hex& topHex = gameState.GetMap().GetHex(MapPos(0, 2));
	topHex.AddShip(ShipType::Interceptor, topHex.GetColour() == Colour::Red ? Colour::Blue : Colour::Red);

	teamstate1.SetPassed(true);
	teamstate2.SetPassed(true);

	game.GetActionPhase().FinishTurn(session);
	game.GetActionPhase().FinishTurn(session);

	Dice dice;
	dice.Add(DiceColour::Yellow, 100);

	while (game.HasBattle())
	{
		session.DoAndPushRecord(game.GetBattle().CreateAttackRecord(game, dice));
		const_cast<CombatPhase&>(game.GetCombatPhase()).FinishTurn(session);
	}

	game.GetUpkeepPhase().FinishUpkeep(session, player1);
	game.GetUpkeepPhase().FinishUpkeep(session, player2);

}

void Test::AddShipsToCentre(LiveGame& game)
{
	Hex* hex = const_cast<Hex*>(game.GetMap().FindHex(1));
	for (auto& team : game.GetTeams())
	{
		for (auto type : PlayerShipTypesRange())
		{
			int count = 0;
			switch (type)
			{
			case ShipType::Interceptor: count = 8; break;
			//case ShipType::Cruiser: count = 4; break;
			//case ShipType::Dreadnought: count = 2; break;
			//case ShipType::Starbase: count = 4; break;
			}

			for (int i = 0; i < count; ++i)
				hex->AddShip(type, team->GetColour());
		}
	}
}
