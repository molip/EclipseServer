#pragma once

#include "Record.h"

#include <vector> 

enum class TechType;

class StartRoundRecord : public Record
{
public:
	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) override;

	std::vector<TechType> m_techs;
};
