#pragma once

#include "Record.h"

#include <string> 

enum class TechType;

class MessageRecord : public TeamRecord
{
public:
	MessageRecord() {}
	MessageRecord(std::string msg);
	MessageRecord(const std::string& player, Colour colour, std::string msg);

	virtual bool IsMessageRecord() const { return true; }

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	virtual void Apply(bool bDo, Game& game, const RecordContext& context) override;
	virtual std::string GetMessage(const Game& game) const override;

	std::string m_player, m_msg;
};
