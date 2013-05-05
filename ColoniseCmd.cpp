#include "stdafx.h"
#include "ColoniseCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "Player.h"
#include "Race.h"
#include "Game.h"
#include "Map.h"

ColoniseCmd::ColoniseCmd(Player& player) : Cmd(player)
{
	for (auto& h : GetGame().GetMap().GetHexes())
		if (h.second->GetOwner() == &GetTeam())
			if (!h.second->GetAvailableSquares().empty()) // TODO: Check pop cubes
				m_positions.push_back(h.first);
}

void ColoniseCmd::UpdateClient(const Controller& controller) const
{
	controller.SendMessage(Output::ChooseColonisePos(m_positions), m_player);
}

CmdPtr ColoniseCmd::Process(const Input::CmdMessage& msg, const Controller& controller)
{
	auto& m = CastThrow<const Input::CmdColonisePos>(msg);
	AssertThrow("ColoniseCmd::Process: invalid pos index", m.m_iPos == -1 || InRange(m_positions, m.m_iPos));
	
	return CmdPtr(new ColoniseSquaresCmd(m_player, m_positions[m.m_iPos]));
}

//-----------------------------------------------------------------------------

ColoniseSquaresCmd::ColoniseSquaresCmd(Player& player, const MapPos& pos) : Cmd(player)
{
	Init(pos);
}

void ColoniseSquaresCmd::Init(const MapPos& pos)
{
	Hex* pHex = GetGame().GetMap().GetHex(pos);
	AssertThrow("ColoniseSquaresCmd: invalid hex", !!pHex);
	
	auto squares = pHex->GetAvailableSquares();
	AssertThrow("ColoniseSquaresCmd: no squares available", !squares.empty());
	AssertThrow("ColoniseSquaresCmd: hex not owned", pHex->GetOwner() == &GetTeam());

	for (int i = 0; i < (int)SquareType::_Count; ++i)
		m_squareCounts[i] = 0;

	for (auto& pSquare : squares)
	{
		++m_squareCounts[(int)pSquare->GetType()];
		m_moves.push_back(std::make_pair(pSquare, Resource::None));
	}
}

void ColoniseSquaresCmd::UpdateClient(const Controller& controller) const
{
	const Population& pop = GetTeam().GetPopulationTrack().GetPopulation();
	int nShips = GetTeam().GetUnusedColonyShips();
	controller.SendMessage(Output::ChooseColoniseSquares(m_squareCounts, pop, nShips), m_player);
}

CmdPtr ColoniseSquaresCmd::Process(const Input::CmdMessage& msg, const Controller& controller)
{
	auto& m = CastThrow<const Input::CmdColoniseSquares>(msg);

	Population fixed = m.m_fixed, grey = m.m_grey, orbital = m.m_orbital;
	Population moved;

	AssertThrow("ColoniseSquaresCmd::Process: no cubes specified", !fixed.IsEmpty() || !grey.IsEmpty() || !orbital.IsEmpty());

	for (auto& move : m_moves)
	{
		move.second  = Resource::None;
		SquareType type = move.first->GetType();
		switch (type)
		{
			case SquareType::Money: 
			case SquareType::Science: 
			case SquareType::Materials: 
			{
				Resource r = SquareTypeToResource(type);
				if (fixed[r])
					--fixed[r], move.second = r;
				break;
			}
			case SquareType::Any:
				for (auto r : EnumRange<Resource>())
					if (grey[r])
						--grey[r], move.second = r;
				break;
			case SquareType::Orbital:
				for (auto r : EnumRange<Resource>(Resource::Money, Resource::Materials))
					if (orbital[r])
						--orbital[r], move.second = r;
		}
	}

	AssertThrow("ColoniseSquaresCmd::Process: not enough squares", fixed.IsEmpty() || grey.IsEmpty() || orbital.IsEmpty());

	Team& team = GetTeam();
	for (auto& move : m_moves)
		if (move.second != Resource::None)
		{
			move.first->SetOwner(&team);
			team.GetPopulationTrack().Remove(move.second, 1);
		}

	controller.SendMessage(Output::UpdateMap(GetGame()), GetGame());

	return nullptr;
}

void ColoniseSquaresCmd::Undo(const Controller& controller)
{
	Team& team = GetTeam();
	for (auto& move : m_moves)
		if (move.second != Resource::None)
		{
			move.first->SetOwner(nullptr);
			team.GetPopulationTrack().Add(move.second, 1);
		}
	controller.SendMessage(Output::UpdateMap(GetGame()), GetGame());
}
