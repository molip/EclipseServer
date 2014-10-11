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

ColoniseCmd::ColoniseCmd(Colour colour, const LiveGame& game) : Cmd(colour)
{
	const Team& team = GetTeam(game);
	for (auto& h : game.GetMap().GetHexes())
		if (h.second->IsOwnedBy(team))
			if (!h.second->GetAvailableSquares(team).empty()) // TODO: Check pop cubes
				m_positions.push_back(h.first);
}

void ColoniseCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseColonisePos(m_positions), GetPlayer(game));
}

CmdPtr ColoniseCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdColonisePos>(msg);
	VerifyInput("ColoniseCmd::Process: invalid pos index", m.m_iPos == -1 || InRange(m_positions, m.m_iPos));
	
	return CmdPtr(new ColoniseSquaresCmd(m_colour, session.GetGame(), m_positions[m.m_iPos]));
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

class ColoniseRecord : public TeamRecord
{
public:
	ColoniseRecord() : m_hexId(0) {}
	ColoniseRecord(Colour colour, const MapPos& pos) : TeamRecord(colour), m_pos(pos), m_hexId(0) {}
	void AddMove(SquareType st, Resource r) { m_moves.push_back(std::make_pair(st, r)); }

	virtual void Save(Serial::SaveNode& node) const override 
	{
		__super::Save(node);
		node.SaveType("pos", m_pos);
		node.SaveType("hex_id", m_hexId);
		node.SavePairs("moves", m_moves, Serial::EnumSaver(), Serial::EnumSaver());
	}
	
	virtual void Load(const Serial::LoadNode& node) override 
	{
		__super::Load(node);
		node.LoadType("pos", m_pos);
		node.LoadType("hex_id", m_hexId);
		node.LoadPairs("moves", m_moves, Serial::EnumLoader(), Serial::EnumLoader());
	}

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) override
	{
		Hex& hex = game.GetMap().GetHex(m_pos);
		m_hexId = hex.GetID();

		Team& team = game.GetTeam(m_colour);
		for (auto& move : m_moves)
			if (move.second != Resource::None)
			{
				Square* pSquare = hex.FindSquare(move.first, !bDo);
				VerifyModel("ColoniseRecord::Apply: square not found", !!pSquare);
				pSquare->SetOccupied(bDo);
				team.GetPopulationTrack().Add(move.second, bDo ? -1 : 1);
			}

		int nMoves = m_moves.size();
		team.UseColonyShips(bDo ? nMoves : -nMoves);

		controller.SendMessage(Output::UpdateMap(game), game);
		controller.SendMessage(Output::UpdatePopulationTrack(team), game);
		controller.SendMessage(Output::UpdateColonyShips(team), game);
	}

	virtual std::string GetTeamMessage() const
	{
		return FormatString("colonised hex %0", m_hexId);
	}

	MapPos m_pos;
	std::vector<std::pair<SquareType, Resource>> m_moves;
	int m_hexId;
};

REGISTER_DYNAMIC(ColoniseRecord)

//-----------------------------------------------------------------------------

ColoniseSquaresCmd::ColoniseSquaresCmd(Colour colour, const LiveGame& game, const MapPos& pos) : Cmd(colour), m_pos(pos)
{
	const Hex& hex = game.GetMap().GetHex(pos);
	
	auto squares = hex.GetAvailableSquares(GetTeam(game));
	VerifyInput("ColoniseSquaresCmd: no squares available", !squares.empty());

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

CmdPtr ColoniseSquaresCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdColoniseSquares>(msg);

	Population fixed = m.m_fixed, grey = m.m_grey, orbital = m.m_orbital;

	VerifyInput("ColoniseSquaresCmd::Process: no cubes specified", !fixed.IsEmpty() || !grey.IsEmpty() || !orbital.IsEmpty());
	VerifyInput("ColoniseSquaresCmd::Process: not enough ships",
		fixed.GetTotal() + grey.GetTotal() + orbital.GetTotal() <= GetTeam(session.GetGame()).GetUnusedColonyShips());

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
				for (auto r : OrbitalResourcesRange())
					if (orbital[r])
						--orbital[r], res = r;
		}
		if (res != Resource::None)
			pRec->AddMove(s, res);
	}

	VerifyInput("ColoniseSquaresCmd::Process: not enough squares", fixed.IsEmpty() || grey.IsEmpty() || orbital.IsEmpty());

	DoRecord(RecordPtr(pRec), session);

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
