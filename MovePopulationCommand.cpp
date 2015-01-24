#include "stdafx.h"
#include "ColoniseCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "Player.h"
#include "Race.h"
#include "LiveGame.h"
#include "Map.h"
#include "Record.h"
#include "CommitSession.h"

MovePopulationCommand::MovePopulationCommand(Colour colour) : Cmd(colour)
{
}

// Allocate population cubes to squares.
MovePopulationCommand::Moves MovePopulationCommand::GetMoves(const Population& pop, const SquareCounts& squareCounts)
{
	Moves moves;

	Population popLeft = pop;
	SquareCounts squareCountsLeft = squareCounts;

	for (auto&& s : EnumRange<SquareType>())
	{
		int& squareCount = squareCountsLeft[s];
		for (auto&& r : EnumRange<Resource>())
		{
			if (pop[r] && squareCount)
			{
				if (s == SquareType::Any ? true :
					s == SquareType::Orbital ? Resources::IsOrbitalType(r) :
					SquareTypeToResource(s) == r)
				{
					int count = std::min(pop[r], squareCount);
					popLeft[r] -= count;
					squareCount -= count;
					moves.push_back(Move(r, s, count));
				}
			}
		}
	}
	
	VERIFY_INPUT_MSG("not enough squares", popLeft.IsEmpty());
	return moves;
}

void MovePopulationCommand::Move::Save(Serial::SaveNode& node) const
{
	node.SaveEnum("pop_type", popType);
	node.SaveEnum("square_type", squareType);
	node.SaveType("count", count);
}

void MovePopulationCommand::Move::Load(const Serial::LoadNode& node)
{
	node.LoadEnum("pop_type", popType);
	node.LoadEnum("square_type", squareType);
	node.LoadType("count", count);
}
