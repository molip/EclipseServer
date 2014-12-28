#include "stdafx.h"
#include "AttackPopulationRecord.h"
#include "Team.h"
#include "Game.h"
#include "Controller.h"
#include "Output.h"
#include "EnumTraits.h"

AttackPopulationRecord::AttackPopulationRecord() : m_cubeCount(0)
{
}

AttackPopulationRecord::AttackPopulationRecord(const Battle::PopulationHits& hits) : AttackPopulationRecord()
{
	m_hits = hits;
}

void AttackPopulationRecord::Apply(bool bDo, const RecordContext& context)
{
	Game& game = context.GetGame();
	GameState& gameState = context.GetGameState();

	Battle& battle = gameState.GetBattle();
	Hex* hex = gameState.GetMap().FindHex(battle.GetHexId());
	VERIFY_MODEL(!!hex && hex->GetFleets().size() == 1);

	auto& squares = hex->GetSquares();

	if (m_cubeCount == 0) // First time. 
	{
		m_firingColour = hex->GetFleets().front().GetColour();
		m_targetColour = hex->GetColour();

		if (!m_hits.autoHit)
			m_firingShipType = battle.GetCurrentGroup().shipType;

		VERIFY_MODEL(m_firingColour != Colour::None && m_targetColour != Colour::None);
		VERIFY_MODEL(!m_hits.autoHit || m_hits.empty());

		for (size_t i = 0; i < squares.size(); ++i)
			if (squares[i].IsOccupied())
			{
				++m_cubeCount;
				if (m_hits.autoHit)
					m_hits.push_back(i);
			}

		VERIFY_MODEL(m_cubeCount > 0);
	}

	for (int squareIndex : m_hits)
		squares[squareIndex].SetOccupied(!bDo);

	if (!m_hits.empty())
		context.SendMessage(Output::UpdateMap(game));
}

std::string AttackPopulationRecord::GetMessage(const Game& game) const
{
	std::string msg;
		
	if (m_hits.autoHit)
		msg = ::FormatString("%0 used neutron bombs", ::EnumToString(m_firingColour));
	else
		msg = ::FormatString("%0 %1 fired cannons, and hit %2/%3 %4 populations",
			::EnumToString(m_firingColour), ::EnumToString(m_firingShipType), m_hits.size(), m_cubeCount, ::EnumToString(m_targetColour));

	if (m_hits.size() == m_cubeCount)
		msg += "\nAll populations destroyed!";

	return msg;
}

void AttackPopulationRecord::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveClass("hits", m_hits);
	node.SaveType("cube_count", m_cubeCount);
}

void AttackPopulationRecord::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadClass("hits", m_hits);
	node.LoadType("cube_count", m_cubeCount);
}

REGISTER_DYNAMIC(AttackPopulationRecord)
