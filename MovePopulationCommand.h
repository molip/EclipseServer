#pragma once

#include "Cmd.h"
#include "Types.h"

#include <vector> 
#include <set> 

enum class Resource;
class Population;

class MovePopulationCommand : public Cmd
{
public:
	MovePopulationCommand() {}
	MovePopulationCommand(Colour colour);

	struct Move
	{
		Move() : popType(), squareType(), count(0) {}
		Move(Resource _popType, SquareType _squareType, int _count) : popType(_popType), squareType(_squareType), count(_count) {}
	
		void Save(Serial::SaveNode& node) const;
		void Load(const Serial::LoadNode& node);
		
		Resource popType;
		SquareType squareType;
		int count;
	};

	typedef std::vector<Move> Moves;

protected:
	static Moves GetMoves(const Population& pop, const SquareCounts& squareCounts);
};

