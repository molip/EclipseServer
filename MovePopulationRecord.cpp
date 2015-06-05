#include "stdafx.h"
#include "MovePopulationRecord.h"
#include "Output.h"

MovePopulationRecord::MovePopulationRecord()
{
}

MovePopulationRecord::MovePopulationRecord(Colour colour, const MovePopulationCommand::Moves& moves) :
	TeamRecord(colour), m_moves(moves)
{
}

void MovePopulationRecord::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveCntr("moves", m_moves, Serial::ClassSaver());
}

void MovePopulationRecord::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadCntr("moves", m_moves, Serial::ClassLoader());
}

void MovePopulationRecord::Update(const Game& game, const Team& team, const RecordContext& context) const
{
	context.SendMessage(Output::UpdatePopulationTrack(team));
}
