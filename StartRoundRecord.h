#pragma once

#include "Record.h"
#include "Resources.h"

#include <map> 

enum class TechType;

class StartRoundRecord : public Record
{
public:
	StartRoundRecord();

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	virtual void Apply(bool bDo, const Game& game, GameState& gameState) override;
	virtual void Update(const Game& game, const RecordContext& context) const override;
	virtual std::string GetMessage(const Game& game) const override;

	struct TeamData
	{
		int actions, colonyShips;

		void Save(Serial::SaveNode& node) const;
		void Load(const Serial::LoadNode& node);
	};

	std::map<Colour, TeamData> m_teamData;
	int m_round;
};
