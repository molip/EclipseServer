#include "stdafx.h"
#include "Record.h"
#include "Team.h"
#include "Serial.h"

Record::Record() : m_colour(Colour::None) {}
Record::Record(Colour colour) : m_colour(colour) {}

Record::~Record() {}

void Record::Save(Serial::SaveNode& node) const 
{
	node.SaveEnum("colour", m_colour);
}
	
void Record::Load(const Serial::LoadNode& node) 
{
	node.LoadEnum("colour", m_colour);
}
