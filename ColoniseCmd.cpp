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
	VERIFY_INPUT_MSG("invalid pos index", m.m_iPos == -1 || InRange(m_positions, m.m_iPos));
	
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
	virtual void Apply(bool bDo, Game& game, const RecordContext& context) override
	{
		Hex& hex = game.GetMap().GetHex(m_pos);
		m_hexId = hex.GetID();

		Team& team = game.GetTeam(m_colour);
		for (auto& move : m_moves)
			if (move.second != Resource::None)
			{
				Square* pSquare = hex.FindSquare(move.first, !bDo);
				VERIFY_MODEL_MSG("square not found", !!pSquare);
				pSquare->SetOccupied(bDo);
				team.GetPopulationTrack().Add(move.second, bDo ? -1 : 1);
			}

		int nMoves = m_moves.size();
		team.UseColonyShips(bDo ? nMoves : -nMoves);

		context.SendMessage(Output::UpdateMap(game));
		context.SendMessage(Output::UpdatePopulationTrack(team));
		context.SendMessage(Output::UpdateColonyShips(team));
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
	VERIFY_INPUT_MSG("no squares available", !squares.empty());

	for (auto& pSquare : squares)
		++m_squareCounts[pSquare->GetType()];
}

void ColoniseSquaresCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	auto& team = GetTeam(game);
	const Population& pop = team.GetPopulationTrack().GetPopulation();
	int nShips = team.GetUnusedColonyShips();
	controller.SendMessage(Output::ChooseColoniseSquares(m_pos, m_squareCounts, pop, nShips), GetPlayer(game));
}

CmdPtr ColoniseSquaresCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdColoniseSquares>(msg);

	Population pop = m.m_moved;

	VERIFY_INPUT_MSG("no moves specified", !pop.IsEmpty());
	VERIFY_INPUT_MSG("not enough ships", pop.GetTotal() <= GetTeam(session.GetGame()).GetUnusedColonyShips());

	ColoniseRecord* pRec = new ColoniseRecord(m_colour, m_pos);

	// Allocate population cubes to squares.
	auto squareCounts = m_squareCounts;
	for (auto&& s : EnumRange<SquareType>())
	{
		int& squareCount = squareCounts[s];
		for (auto&& r : EnumRange<Resource>())
		{
			if (pop[r] && squareCount)
			{
				if (s == SquareType::Any ? true :
					s == SquareType::Orbital ? Resources::IsOrbitalType(r) :
					SquareTypeToResource(s) == r)
				{
					int move = std::min(pop[r], squareCount);
					pop[r] -= move;
					squareCount -= move;
					pRec->AddMove(s, r);
				}
			}
		}
	}
	
	VERIFY_INPUT_MSG("not enough squares", pop.IsEmpty());

	DoRecord(RecordPtr(pRec), session);

	return nullptr;
}

void ColoniseSquaresCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveType("pos", m_pos);
	node.SaveArray("square_counts", m_squareCounts, Serial::TypeSaver());
}

void ColoniseSquaresCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadType("pos", m_pos);
	node.LoadArray("square_counts", m_squareCounts, Serial::TypeLoader());
}

REGISTER_DYNAMIC(ColoniseSquaresCmd)
