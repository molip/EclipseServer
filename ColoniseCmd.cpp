#include "stdafx.h"
#include "ColoniseCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "Player.h"
#include "Race.h"
#include "LiveGame.h"
#include "Map.h"
#include "MoveHexPopulationRecord.h"
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

class ColoniseRecord : public MoveHexPopulationRecord
{
public:
	ColoniseRecord() {}
	ColoniseRecord(Colour colour, const MapPos& pos, const MovePopulationCommand::Moves& moves) : MoveHexPopulationRecord(colour, pos, moves) {}

private:
	virtual bool IsToPlanet() const override { return true; }

	virtual void Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState) override
	{
		__super::Apply(bDo, game, team, gameState, teamState);
		for (auto& move : m_moves)
			teamState.UseColonyShips(bDo ? move.count : -move.count);
	}

	void Update(const Game& game, const Team& team, const RecordContext& context) const
	{
		__super::Update(game, team, context);
		context.SendMessage(Output::UpdateColonyShips(team));
	}

	virtual std::string GetTeamMessage() const
	{
		return FormatString("colonised hex %0", m_hexId);
	}
};

REGISTER_DYNAMIC(ColoniseRecord)

//-----------------------------------------------------------------------------

ColoniseSquaresCmd::ColoniseSquaresCmd(Colour colour, const LiveGame& game, const MapPos& pos) : MovePopulationCommand(colour), m_pos(pos)
{
}

SquareCounts ColoniseSquaresCmd::GetSquareCounts(const LiveGame& game) const
{
	return game.GetMap().GetHex(m_pos).GetAvailableSquareCounts(GetTeam(game));
}

void ColoniseSquaresCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	auto& team = GetTeam(game);
	const Population& pop = team.GetPopulationTrack().GetPopulation();
	int nShips = team.GetUnusedColonyShips();
	controller.SendMessage(Output::ChooseColoniseSquares(m_pos, GetSquareCounts(game), pop, nShips), GetPlayer(game));
}

Cmd::ProcessResult ColoniseSquaresCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdColoniseSquares>(msg);

	Population pop = m.m_moved;

	VERIFY_INPUT_MSG("no moves specified", !pop.IsEmpty());
	VERIFY_INPUT_MSG("not enough ships", pop.GetTotal() <= GetTeam(session.GetGame()).GetUnusedColonyShips());

	Moves moves = GetMoves(pop, GetSquareCounts(session.GetGame()));
	DoRecord(RecordPtr(new ColoniseRecord(m_colour, m_pos, moves)), session);

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
