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

ColoniseCmd::ColoniseCmd(Colour colour, LiveGame& game) : Cmd(colour)
{
	Team& team = GetTeam(game);
	for (auto& h : game.GetMap().GetHexes())
		if (h.second->IsOwnedBy(team))
			if (!h.second->GetAvailableSquares(team).empty()) // TODO: Check pop cubes
				m_positions.push_back(h.first);
}

void ColoniseCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseColonisePos(m_positions), GetPlayer(game));
}

CmdPtr ColoniseCmd::Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game)
{
	auto& m = CastThrow<const Input::CmdColonisePos>(msg);
	AssertThrow("ColoniseCmd::Process: invalid pos index", m.m_iPos == -1 || InRange(m_positions, m.m_iPos));
	
	return CmdPtr(new ColoniseSquaresCmd(m_colour, game, m_positions[m.m_iPos]));
}

void ColoniseCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveCntr("positions", m_positions, Serial::TypeSaver());
}

void ColoniseCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadCntr("positions", m_positions, Serial::TypeLoader());
}

REGISTER_DYNAMIC(ColoniseCmd)

//-----------------------------------------------------------------------------

class ColoniseRecord : public Record
{
public:
	ColoniseRecord() {}
	ColoniseRecord(Colour colour, const MapPos& pos) : Record(colour), m_pos(pos) {}
	void AddMove(SquareType st, Resource r) { m_moves.push_back(std::make_pair(st, r)); }

	virtual void Save(Serial::SaveNode& node) const override 
	{
		__super::Save(node);
		node.SaveType("pos", m_pos);
		node.SavePairs("moves", m_moves, Serial::EnumSaver(), Serial::EnumSaver());
	}
	
	virtual void Load(const Serial::LoadNode& node) override 
	{
		__super::Load(node);
		node.LoadType("pos", m_pos);
		node.LoadPairs("moves", m_moves, Serial::EnumLoader(), Serial::EnumLoader());
	}

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) override
	{
		Hex& hex = game.GetMap().GetHex(m_pos);

		Team& team = game.GetTeam(m_colour);
		for (auto& move : m_moves)
			if (move.second != Resource::None)
			{
				Square* pSquare = hex.FindSquare(move.first, !bDo);
				AssertThrow("ColoniseRecord::Apply: square not found", !!pSquare);
				pSquare->SetOccupied(bDo);
				team.GetPopulationTrack().Add(move.second, bDo ? -1 : 1);
			}
		controller.SendMessage(Output::UpdateMap(game), game);
		controller.SendMessage(Output::UpdatePopulationTrack(team), game);
	}

	MapPos m_pos;
	std::vector<std::pair<SquareType, Resource>> m_moves;
};

REGISTER_DYNAMIC(ColoniseRecord)

//-----------------------------------------------------------------------------

ColoniseSquaresCmd::ColoniseSquaresCmd(Colour colour, LiveGame& game, const MapPos& pos) : Cmd(colour), m_pos(pos)
{
	Hex& hex = game.GetMap().GetHex(pos);
	
	auto squares = hex.GetAvailableSquares(GetTeam(game));
	AssertThrow("ColoniseSquaresCmd: no squares available", !squares.empty());

	for (int i = 0; i < (int)SquareType::_Count; ++i)
		m_squareCounts[i] = 0;

	for (auto& pSquare : squares)
	{
		++m_squareCounts[(int)pSquare->GetType()];
		m_squares.push_back(pSquare->GetType());
	}
}

void ColoniseSquaresCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	auto& team = GetTeam(game);
	const Population& pop = team.GetPopulationTrack().GetPopulation();
	int nShips = team.GetUnusedColonyShips();
	controller.SendMessage(Output::ChooseColoniseSquares(m_squareCounts, pop, nShips), GetPlayer(game));
}

CmdPtr ColoniseSquaresCmd::Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game)
{
	auto& m = CastThrow<const Input::CmdColoniseSquares>(msg);

	Population fixed = m.m_fixed, grey = m.m_grey, orbital = m.m_orbital;

	AssertThrow("ColoniseSquaresCmd::Process: no cubes specified", !fixed.IsEmpty() || !grey.IsEmpty() || !orbital.IsEmpty());

	ColoniseRecord* pRec = new ColoniseRecord(m_colour, m_pos);

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

	pRec->Do(game, controller);
	game.PushRecord(RecordPtr(pRec));

	return nullptr;
}

void ColoniseSquaresCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveCntr("squares", m_squares, Serial::EnumSaver());
	node.SaveType("pos", m_pos);
	node.SaveArray("square_counts", m_squareCounts, Serial::TypeSaver());
}

void ColoniseSquaresCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);

	node.LoadCntr("squares", m_squares, Serial::EnumLoader());
	node.LoadType("pos", m_pos);
	node.LoadArray("square_counts", m_squareCounts, Serial::TypeLoader());
}

REGISTER_DYNAMIC(ColoniseSquaresCmd)
