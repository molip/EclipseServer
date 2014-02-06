#pragma once

#include "Record.h"
#include "Resources.h"

#include <vector> 

enum class TechType;

class StartRoundRecord : public Record
{
public:
	StartRoundRecord();

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) override;

	struct TeamData
	{
		int actions, colonyShips;
		Storage income;
	};

	std::vector<TechType> m_techs;
	std::vector<TeamData> m_teamData;
};
