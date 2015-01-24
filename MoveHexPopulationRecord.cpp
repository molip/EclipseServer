#include "stdafx.h"
#include "MoveHexPopulationRecord.h"
#include "Output.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Map.h"

MoveHexPopulationRecord::MoveHexPopulationRecord() : m_hexId(0) 
{
}

MoveHexPopulationRecord::MoveHexPopulationRecord(Colour colour, const MapPos& pos, const MovePopulationCommand::Moves& moves) :
	MovePopulationRecord(colour, moves), m_pos(pos), m_hexId(0) {}

void MoveHexPopulationRecord::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveType("pos", m_pos);
	node.SaveType("hex_id", m_hexId);
}

void MoveHexPopulationRecord::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadType("pos", m_pos);
	node.LoadType("hex_id", m_hexId);
}

void MoveHexPopulationRecord::Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState)
{
	Hex& hex = gameState.GetMap().GetHex(m_pos);
	m_hexId = hex.GetID();

	bool toPlanet = bDo == IsToPlanet();

	for (auto& move : m_moves)
	{
		for (int i = 0; i < move.count; ++i)
		{
			Square* pSquare = hex.FindSquare(move.squareType, !toPlanet);
			VERIFY_MODEL_MSG("square not found", !!pSquare);
			pSquare->SetOccupied(toPlanet);
		}
		teamState.GetPopulationTrack().Add(move.popType, toPlanet ? -move.count : move.count);
	}
}

void MoveHexPopulationRecord::Update(const Game& game, const Team& team, const RecordContext& context) const
{
	__super::Update(game, team, context);
	context.SendMessage(Output::UpdateMap(context.GetGame()));
}
