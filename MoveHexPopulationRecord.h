#pragma once

#include "MovePopulationRecord.h"
#include "MapPos.h"

class MoveHexPopulationRecord : public MovePopulationRecord
{
public:
	MoveHexPopulationRecord();
	MoveHexPopulationRecord(Colour colour, const MapPos& pos, const MovePopulationCommand::Moves& moves);
	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

protected:
	virtual void Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState) override;
	virtual void Update(const Game& game, const Team& team, const RecordContext& context) const override;

	virtual bool IsToPlanet() const = 0;

	MapPos m_pos;
	int m_hexId;
};
