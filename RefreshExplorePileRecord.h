#pragma once

#include "Record.h"
#include "Resources.h"

#include <map> 

enum class TechType;
enum class HexRing;

class RefreshExplorePileRecord : public Record
{
public:
	RefreshExplorePileRecord();
	RefreshExplorePileRecord(HexRing ring);

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

	HexRing m_ring;
	std::vector<int> m_discardPile, m_newPile;
};
