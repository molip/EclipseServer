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
}

std::vector<MapPos> ColoniseCmd::GetPositions(const LiveGame& game) const
{
	std::vector<MapPos> positions;
	const Team& team = GetTeam(game);
	for (auto& h : game.GetMap().GetHexes())
		if (h.second->IsOwnedBy(team))
			if (!h.second->GetAvailableSquares(team).empty()) // TODO: Check pop cubes
				positions.push_back(h.first);
	return positions;
}

void ColoniseCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseColonisePos(GetPositions(game)), GetPlayer(game));
}

Cmd::ProcessResult ColoniseCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdColonisePos>(msg);
	
	auto positions = GetPositions(session.GetGame());
	VERIFY_INPUT_MSG("invalid pos index", m.m_iPos == -1 || InRange(positions, m.m_iPos));
	
	return ProcessResult(new ColoniseSquaresCmd(m_colour, session.GetGame(), positions[m.m_iPos]));
}

void ColoniseCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
}

void ColoniseCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
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
	virtual void Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState) override
	{
		Hex& hex = gameState.GetMap().GetHex(m_pos);
		m_hexId = hex.GetID();

		for (auto& move : m_moves)
			if (move.second != Resource::None)
			{
				Square* pSquare = hex.FindSquare(move.first, !bDo);
				VERIFY_MODEL_MSG("square not found", !!pSquare);
				pSquare->SetOccupied(bDo);
				teamState.GetPopulationTrack().Add(move.second, bDo ? -1 : 1);
			}

		int nMoves = m_moves.size();
		teamState.UseColonyShips(bDo ? nMoves : -nMoves);
	}

	void Update(const Game& game, const Team& team, const RecordContext& context) const
	{
		context.SendMessage(Output::UpdateMap(context.GetGame()));
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
}

SquareCounts ColoniseSquaresCmd::GetSquareCounts(const LiveGame& game) const
{
	auto squares = game.GetMap().GetHex(m_pos).GetAvailableSquares(GetTeam(game));
	VERIFY_INPUT_MSG("no squares available", !squares.empty());

	SquareCounts squareCounts;
	for (auto& pSquare : squares)
		++squareCounts[pSquare->GetType()];

	return squareCounts;
}

void ColoniseSquaresCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	auto& team = GetTeam(game);
	const Population& pop = team.GetPopulationTrack().GetPopulation();
	int nShips = team.GetUnusedColonyShips();
	SquareCounts squareCounts = GetSquareCounts(game);
	controller.SendMessage(Output::ChooseColoniseSquares(m_pos, squareCounts, pop, nShips), GetPlayer(game));
}

Cmd::ProcessResult ColoniseSquaresCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdColoniseSquares>(msg);

	Population pop = m.m_moved;

	VERIFY_INPUT_MSG("no moves specified", !pop.IsEmpty());
	VERIFY_INPUT_MSG("not enough ships", pop.GetTotal() <= GetTeam(session.GetGame()).GetUnusedColonyShips());

	ColoniseRecord* pRec = new ColoniseRecord(m_colour, m_pos);

	// Allocate population cubes to squares.
	auto squareCounts = GetSquareCounts(session.GetGame());
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
}

void ColoniseSquaresCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadType("pos", m_pos);
}

REGISTER_DYNAMIC(ColoniseSquaresCmd)
