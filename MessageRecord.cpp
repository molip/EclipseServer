#include "stdafx.h"
#include "MessageRecord.h"
#include "Serial.h"
#include "Team.h"

MessageRecord::MessageRecord(std::string msg) : TeamRecord(Colour::None), m_msg(msg)
{
}

MessageRecord::MessageRecord(const std::string& player, Colour colour, std::string msg) : 
TeamRecord(colour), m_player(player), m_msg(msg)
{
}

void MessageRecord::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveType("player", m_player);
	node.SaveType("msg", m_msg);
}

void MessageRecord::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadType("player", m_player);
	node.LoadType("msg", m_msg);
}

void MessageRecord::Apply(bool bDo, const Team& team, TeamState& teamState, const RecordContext& context)
{
}

std::string MessageRecord::GetMessage(const Game& game) const
{
	std::string prefix;
	if (!m_player.empty())
	{
		if (m_colour == Colour::None)
			prefix = FormatString("%0 said: ", m_player);
		else 
			prefix = FormatString("%0 (%1) said: ", m_player, ::EnumToString(m_colour));
	}

	return prefix + m_msg;
}

REGISTER_DYNAMIC(MessageRecord)

