#include "stdafx.h"
#include "Ship.h"

Ship::Ship(const Blueprint& blueprint) : m_blueprint(blueprint), m_layout(blueprint.GetType())
{
}
