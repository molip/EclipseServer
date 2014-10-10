#pragma once

#include "Record.h"
#include "Battle.h"

enum class ShipType;

class AttackRecord : public Record
{
public:
	AttackRecord();
	AttackRecord(const Battle::Hits& hits);
	
private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller);
	virtual std::string GetMessage(const Game& game) const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	Battle::Hits m_hits;
	Colour m_firingColour, m_targetColour;
	ShipType m_firingShipType;
	bool m_missilePhase;
	std::set<int> m_killIndices;
	//Battle::Turn m_oldTurn;
};
