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

ColoniseCmd::ColoniseCmd(Player& player) : Cmd(player)
{
	for (auto& h : GetGame().GetMap().GetHexes())
		if (h.second->IsOwnedBy(GetTeam()))
			if (!h.second->GetAvailableSquares(GetTeam()).empty()) // TODO: Check pop cubes
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

class ColoniseRecord : public Record
{
public:
	ColoniseRecord(Colour colour, const MapPos& pos) : m_colour(colour), m_pos(pos) {}
	void AddMove(SquareType st, Resource r) { m_moves.push_back(std::make_pair(st, r)); }

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) override
	{
		Hex* pHex = game.GetMap().GetHex(m_pos);
		AssertThrow("ColoniseRecord::Apply: hex not found", !!pHex);

		Team& team = game.GetTeam(m_colour);
		for (auto& move : m_moves)
			if (move.second != Resource::None)
			{
				Square* pSquare = pHex->FindSquare(move.first, !bDo);
				AssertThrow("ColoniseRecord::Apply: square not found", !!pSquare);
				pSquare->SetOccupied(bDo);
				team.GetPopulationTrack().Add(move.second, bDo ? -1 : 1);
			}
		controller.SendMessage(Output::UpdateMap(game), game);
		controller.SendMessage(Output::UpdatePopulationTrack(team), game);
	}

	Colour m_colour;
	MapPos m_pos;
	std::vector<std::pair<SquareType, Resource>> m_moves;
};

//-----------------------------------------------------------------------------

ColoniseSquaresCmd::ColoniseSquaresCmd(Player& player, const MapPos& pos) : Cmd(player), m_pos(pos)
{
	Init(pos);
}

void ColoniseSquaresCmd::Init(const MapPos& pos)
{
	Hex* pHex = GetGame().GetMap().GetHex(pos);
	AssertThrow("ColoniseSquaresCmd: invalid hex", !!pHex);
	
	auto squares = pHex->GetAvailableSquares(GetTeam());
	AssertThrow("ColoniseSquaresCmd: no squares available", !squares.empty());

	for (int i = 0; i < (int)SquareType::_Count; ++i)
		m_squareCounts[i] = 0;

	for (auto& pSquare : squares)
	{
		++m_squareCounts[(int)pSquare->GetType()];
		m_squares.push_back(pSquare->GetType());
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

	AssertThrow("ColoniseSquaresCmd::Process: no cubes specified", !fixed.IsEmpty() || !grey.IsEmpty() || !orbital.IsEmpty());

	ColoniseRecord* pRec = new ColoniseRecord(GetTeam().GetColour(), m_pos);

	// Allocate population cubes to squares.
	for (auto& s : m_squares)
	{
		Resource res = Resource::None;
		switch (s)
		{
			case SquareType::Money: 
			case SquareType::Science: 
			case SquareType::Materials: 
			{
				Resource r = SquareTypeToResource(s);
				if (fixed[r])
					--fixed[r], res = r;
				break;
			}
			case SquareType::Any:
				for (auto r : EnumRange<Resource>())
					if (grey[r])
						--grey[r], res = r;
				break;
			case SquareType::Orbital:
				for (auto r : EnumRange<Resource>(Resource::Money, Resource::Materials))
					if (orbital[r])
						--orbital[r], res = r;
		}
		if (res != Resource::None)
			pRec->AddMove(s, res);
	}

	AssertThrow("ColoniseSquaresCmd::Process: not enough squares", fixed.IsEmpty() || grey.IsEmpty() || orbital.IsEmpty());

	pRec->Do(GetGame(), controller);
	GetGame().PushRecord(RecordPtr(pRec));

	return nullptr;
}

void ColoniseSquaresCmd::Undo(const Controller& controller)
{
	RecordPtr pRec = GetGame().PopRecord();
	pRec->Undo(GetGame(), controller);
}
