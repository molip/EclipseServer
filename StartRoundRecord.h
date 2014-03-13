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
	virtual std::string GetMessage(const Game& game) const override;

	struct TeamData
	{
		int actions, colonyShips;
		Storage income;

		void Save(Serial::SaveNode& node) const;
		void Load(const Serial::LoadNode& node);
	};

	std::vector<TechType> m_techs;
	std::vector<TeamData> m_teamData;
	int m_round;
};
