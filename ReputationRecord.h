#pragma once

#include "Record.h"

enum class Colour;

class ReputationRecord : public Record
{
public:
	typedef std::map<Colour, int> TileValues;

	ReputationRecord();
	ReputationRecord(const TileValues& values);
	virtual ~ReputationRecord();

	virtual std::string GetMessage(const Game & game) const override;

private:
	virtual void Apply(bool bDo, const Game & game, GameState & gameState) override;
	virtual void Update(const Game& game, const RecordContext& context) const override;

	virtual void Save(Serial::SaveNode & node) const override;
	virtual void Load(const Serial::LoadNode & node) override;

	std::map<Colour, std::pair<int, int>> m_values; // Val, undo.
};

