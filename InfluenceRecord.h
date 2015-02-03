#pragma once

#include "Record.h"

class MapPos;
DEFINE_UNIQUE_PTR(MapPos)
class Hex;
enum class DiscoveryType;

class InfluenceRecord : public TeamRecord
{
public:
	InfluenceRecord();
	InfluenceRecord(Colour colour, const MapPos* pSrcPos, const MapPos* pDstPos);

	DiscoveryType GetDiscovery() const { return m_discovery; }

	virtual void Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState) override;
	virtual void Update(const Game& game, const Team& team, const RecordContext& context) const override;
	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	Hex* TransferDisc(const MapPosPtr& pSrcPos, const MapPosPtr& pDstPos, GameState& gameState, const Team& team, TeamState& teamState);
	virtual std::string GetTeamMessage() const;

	int m_srcID, m_dstID;
	MapPosPtr m_pSrcPos, m_pDstPos;
	DiscoveryType m_discovery;
};
