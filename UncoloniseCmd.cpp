#include "stdafx.h"
#include "UncoloniseCmd.h"
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
#include "Debug.h"
#include "MoveHexPopulationRecord.h"

class UncoloniseRecord : public MoveHexPopulationRecord
{
public:
	UncoloniseRecord() {}
	UncoloniseRecord(Colour colour, const MapPos& pos, const MovePopulationCommand::Moves& moves) : MoveHexPopulationRecord(colour, pos, moves) {}

private:
	virtual bool IsToPlanet() const override { return false; }

	virtual std::string GetTeamMessage() const
	{
		return FormatString("uncolonised hex %0", m_hexId);
	}
};

REGISTER_DYNAMIC(UncoloniseRecord)

//-----------------------------------------------------------------------------

UncoloniseCmd::UncoloniseCmd(Colour colour, const MapPos& pos) : MovePopulationCommand(colour), m_pos(pos)
{
}

SquareCounts UncoloniseCmd::GetSquareCounts(const LiveGame& game) const
{
	return game.GetMap().GetHex(m_pos).GetOccupiedSquareCounts();
}

void UncoloniseCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	auto& team = GetTeam(game);
	Population pop = team.GetPopulationTrack().GetEmptySpaces();
	controller.SendMessage(Output::ChooseUncolonise(GetSquareCounts(game), pop), GetPlayer(game));
}

Cmd::ProcessResult UncoloniseCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdUncolonise>(msg);

	Population pop = m.m_moved;

	const LiveGame& game = session.GetGame();
	auto squareCounts = GetSquareCounts(game);
	VERIFY_INPUT(pop.GetTotal() == squareCounts.GetTotal());

	Moves moves = GetMoves(pop, squareCounts);
	DoRecord(RecordPtr(new UncoloniseRecord(m_colour, m_pos, moves)), session);

	return ProcessResult();
}

void UncoloniseCmd::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveType("pos", m_pos);
}

void UncoloniseCmd::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadType("pos", m_pos);
}

REGISTER_DYNAMIC(UncoloniseCmd)
