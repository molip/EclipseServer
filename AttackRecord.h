#pragma once

#include "Record.h"

enum class ShipType;

class AttackRecord : public Record
{
public:
	AttackRecord();
	
protected:
	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	Colour m_firingColour, m_targetColour;
	ShipType m_firingShipType;
};
