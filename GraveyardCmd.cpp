#include "stdafx.h"
#include "GraveyardCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "Player.h"
#include "Race.h"
#include "LiveGame.h"
#include "Map.h"
#include "DiscoverCmd.h"
#include "Record.h"
#include "CommitSession.h"
#include "GraveyardCmd.h"
#include "Debug.h"
#include "MovePopulationRecord.h"

class GraveyardRecord : public MovePopulationRecord
{
public:
	GraveyardRecord() {}
	GraveyardRecord(Colour colour, const MovePopulationCommand::Moves& moves) : MovePopulationRecord(colour, moves) {}

private:
	virtual void Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState) override
	{
		for (auto& move : m_moves)
		{
			int popDelta = bDo ? move.count : -move.count;
			teamState.m_graveyard[move.squareType] -= popDelta;
			teamState.GetPopulationTrack().Add(move.popType, popDelta);
		}
	}

	virtual std::string GetTeamMessage() const
	{
		return "emptied graveyard";
	}
};

REGISTER_DYNAMIC(GraveyardRecord)

//-----------------------------------------------------------------------------

GraveyardCmd::GraveyardCmd(Colour colour, const LiveGame& game) : MovePopulationCommand(colour)
{
	VERIFY_MODEL(!GetTeam(game).GetGraveyard().IsEmpty());
}

void GraveyardCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	auto& team = GetTeam(game);
	Population pop = team.GetPopulationTrack().GetEmptySpaces();
	controller.SendMessage(Output::ChooseUncolonise(team.GetGraveyard(), pop), GetPlayer(game));
}

Cmd::ProcessResult GraveyardCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdUncolonise>(msg);

	Population pop = m.m_moved;

	const LiveGame& game = session.GetGame();
	auto& team = GetTeam(game);
	VERIFY_INPUT(pop.GetTotal() == team.GetGraveyard().GetTotal());

	auto squareCounts = team.GetGraveyard();
	Moves moves = GetMoves(pop, squareCounts);

	DoRecord(RecordPtr(new GraveyardRecord(m_colour, moves)), session);

	return ProcessResult();
}

void GraveyardCmd::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
}

void GraveyardCmd::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
}

REGISTER_DYNAMIC(GraveyardCmd)
