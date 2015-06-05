#include "stdafx.h"
#include "AttackRecord.h"
#include "Team.h"

AttackRecord::AttackRecord() : m_firingColour(Colour::None), m_targetColour(Colour::None), m_firingShipType(ShipType::None)
{
}

void AttackRecord::Save(Serial::SaveNode& node) const
{
	__super::Save(node);

	node.SaveEnum("firing_colour", m_firingColour);
	node.SaveEnum("target_colour", m_targetColour);
	node.SaveEnum("firing_ship", m_firingShipType);
}

void AttackRecord::Load(const Serial::LoadNode& node)
{
	__super::Load(node);

	node.LoadEnum("firing_colour", m_firingColour);
	node.LoadEnum("target_colour", m_targetColour);
	node.LoadEnum("firing_ship", m_firingShipType);
}
