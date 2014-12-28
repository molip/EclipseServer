#include "stdafx.h"
#include "AttackShipsRecord.h"
#include "Team.h"
#include "Game.h"
#include "Controller.h"
#include "Output.h"
#include "EnumTraits.h"

AttackShipsRecord::AttackShipsRecord() : m_missilePhase(false)
{
}

AttackShipsRecord::AttackShipsRecord(const Battle::Hits& hits) : AttackShipsRecord()
{
	m_hits = hits;
}

void AttackShipsRecord::Apply(bool bDo, const RecordContext& context)
{
	GameState& gameState = context.GetGameState();

	Battle& battle = gameState.GetBattle();

	if (m_firingShipType == ShipType::None)
	{
		m_targetColour = battle.GetTargetColour();
		m_firingColour = battle.GetFiringColour();
		m_firingShipType = battle.GetCurrentGroup().shipType;
		m_missilePhase = battle.IsMissilePhase();
	}

	Hex* hex = gameState.GetMap().FindHex(battle.GetHexId());
	VERIFY_MODEL(!!hex);

	bool updateMap = false;
	for (size_t i = 0; i < m_hits.size(); ++i)
	{
		const auto& hit = m_hits[i];
		
		Battle::Group* group = battle.FindTargetGroup(hit.shipType);
		VERIFY_MODEL(!!group);
		
		if (bDo)
		{
			VERIFY_MODEL(group->lifeCounts[hit.shipIndex] > 0);
			group->lifeCounts[hit.shipIndex] -= hit.dice.GetDamage();

			if (group->lifeCounts[hit.shipIndex] <= 0)
			{
				hex->RemoveShip(hit.shipType, m_targetColour);
				m_killIndices.insert(i);
				updateMap = true;
			}
		}
		else
		{
			if (group->lifeCounts[hit.shipIndex] <= 0)
			{
				hex->AddShip(hit.shipType, m_targetColour);
				updateMap = true;
			}

			group->lifeCounts[hit.shipIndex] += hit.dice.GetDamage();
			VERIFY_MODEL(group->lifeCounts[hit.shipIndex] > 0);
		}
	}

	context.SendMessage(Output::UpdateCombat(context.GetGame(), battle));

	if (updateMap)
		context.SendMessage(Output::UpdateMap(context.GetGame()));
}

std::string AttackShipsRecord::GetMessage(const Game& game) const
{
	std::string firingTeam = m_firingColour == Colour::None ? "" : std::string(::EnumToString(m_firingColour)) + ' ';
	std::string targetTeam = m_targetColour == Colour::None ? "" : std::string(::EnumToString(m_targetColour)) + ' ';

	std::string msg = ::FormatString("%0%1 fired %2, and hit: ",
		firingTeam, ::EnumToString(m_firingShipType), m_missilePhase ? "missiles" : "cannons");
	
	if (m_hits.empty())
		msg += "nothing";
	else
		for (size_t i = 0; i < m_hits.size(); ++i)
		{
			const auto& hit = m_hits[i];
			std::string dead = m_killIndices.count(i) ? ", dead" : "";

			msg += ::FormatString("\n* %0%1 (damage %2%3)",
				targetTeam, ::EnumToString(hit.shipType), hit.dice.GetDamage(), dead);
		}

	return msg;
}

void AttackShipsRecord::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveClass("hits", m_hits);

	// Just for GetMessage.
	node.SaveEnum("firing_colour", m_firingColour);
	node.SaveEnum("target_colour", m_targetColour);
	node.SaveEnum("firing_ship", m_firingShipType);
	node.SaveType("missile_phase", m_missilePhase);
	node.SaveCntr("kill_indices", m_killIndices,  Serial::TypeSaver());
}

void AttackShipsRecord::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadClass("hits", m_hits);

	// Just for GetMessage.
	node.LoadEnum("firing_colour", m_firingColour);
	node.LoadEnum("target_colour", m_targetColour);
	node.LoadEnum("firing_ship", m_firingShipType);
	node.LoadType("missile_phase", m_missilePhase);
	node.LoadCntr("kill_indices", m_killIndices, Serial::TypeLoader());
}

REGISTER_DYNAMIC(AttackShipsRecord)
