#include "stdafx.h"
#include "ShipBattle.h"
#include "App.h"
#include "Hex.h"
#include "Blueprint.h"
#include "LiveGame.h"
#include "Test.h"
#include "AttackShipsRecord.h"

ShipBattle::Hit::Hit() : shipType(ShipType::None), shipIndex(-1) {}
ShipBattle::Hit::Hit(ShipType _shipType, int _shipIndex, const Dice& _dice) :
	shipType(_shipType), shipIndex(_shipIndex), dice(_dice) {}

void ShipBattle::Hit::Save(Serial::SaveNode& node) const
{
	node.SaveEnum("ship_type", shipType);
	node.SaveType("ship_index", shipIndex);
	node.SaveClass("dice", dice);
}

void ShipBattle::Hit::Load(const Serial::LoadNode& node)
{
	node.LoadEnum("ship_type", shipType);
	node.LoadType("ship_index", shipIndex);
	node.LoadClass("dice", dice);
}

void ShipBattle::Hits::Save(Serial::SaveNode& node) const
{
	node.SaveCntr("hits", *this, Serial::ClassSaver());
}

void ShipBattle::Hits::Load(const Serial::LoadNode& node)
{
	node.LoadCntr("hits", *this, Serial::ClassLoader());
}

//-------------------------------------------------------------------

ShipBattle::ShipBattle()
{
}

ShipBattle::ShipBattle(const Hex& hex, const Game& game, const GroupVec& oldGroups) : Battle(hex, game, oldGroups)
{
	VERIFY_MODEL(m_groups.size() >= 2);

	bool missiles = false;
	for (auto& group : m_groups)
	{
		group.hasMissiles = GetBlueprint(game, group.shipType, group.invader).HasMissiles();
		missiles |= group.hasMissiles;
	}

	m_turn.groupIndex = missiles ? FindFirstMissileGroup() : 0;
	m_turn.phase = missiles ? BattlePhase::Missile : BattlePhase::Main;
}

bool ShipBattle::operator==(const Battle& rhs) const
{
	return dynamic_cast<const ShipBattle*>(&rhs) && __super::operator==(rhs);
}

BattlePtr ShipBattle::Clone() const
{
	return BattlePtr(new ShipBattle(*this));
}

void ShipBattle::DoAdvanceTurn(const Game& game)
{
	if (IsMissilePhase())
	{
		VERIFY_MODEL(GetCurrentGroup().hasMissiles);
		GetCurrentGroup().hasMissiles = false;
		m_turn.groupIndex = FindFirstMissileGroup();

		if (m_turn.groupIndex == -1) // Gets advanced below. 
			m_turn.phase = BattlePhase::Main;
	}

	const Hex* hex = game.GetMap().FindHex(m_hexId);

	if (m_turn.phase == BattlePhase::Main) // Advance to next living group (there should always be one).
	{
		if (IsMainPhaseFinished())
		{
			m_turn.groupIndex = -1; // Finished. 
		}
		else
		{
			do
				m_turn.groupIndex = (m_turn.groupIndex + 1) % m_groups.size();
			while (GetCurrentGroup().IsDead()); // TODO: Check cannons. 
		}
	}
}

std::vector<int> ShipBattle::GetShipIndicesWeakestFirst(const Group& group) const
{
	auto pred = [&](int lhs, int rhs) { return group.lifeCounts[lhs] < group.lifeCounts[rhs]; };

	std::vector<int> indices;
	for (int i = 0; i < (int)group.lifeCounts.size(); ++i)
		if (group.lifeCounts[i] > 0) // Ignore dead ones.
			indices.push_back(i);

	std::sort(indices.begin(), indices.end(), pred);
	return indices;
}

std::vector<int> ShipBattle::GetTargetGroupIndicesBiggestFirst() const
{
	auto pred = [&](int lhs, int rhs) { return GetShipTypeSize(m_groups[lhs].shipType) > GetShipTypeSize(m_groups[rhs].shipType); };

	std::vector<int> indices;
	for (int i = 0; i < (int)m_groups.size(); ++i)
		if (m_groups[i].invader != GetCurrentGroup().invader)
			indices.push_back(i);
	std::sort(indices.begin(), indices.end(), pred);
	return indices;
}

ShipBattle::Hits ShipBattle::GetHitsToDestroy(const Group& group, Dice& dice, int toHit) const
{
	const auto shipIndices = GetShipIndicesWeakestFirst(group);

	Hits hits;
	for (int shipIndex : shipIndices)
	{
		int lives = group.lifeCounts[shipIndex];
		Dice used = dice.Remove(lives, toHit);
		if (used.empty())
			break; // Can't destroy any more ships in this group. 

		// We can destroy this ship. 
		int damage = used.GetDamage();
		if (damage > lives) // Don't want to waste damage. Can we destroy a healthier ship? 
		{
			for (int shipIndex2 : shipIndices)
				if (group.lifeCounts[shipIndex2] == damage)
				{
					// We can destroy this one with no waste. 
					shipIndex = shipIndex2;
					lives = damage;
					break;
				}
		}

		hits.push_back(Hit(group.shipType, shipIndex, used));
	}
	return hits;
}

ShipBattle::Hits ShipBattle::AutoAssignHits(const Dice& dice, const Game& game) const
{
	Hits hits;

	Dice remainingDice = dice;

	std::map<int, std::set<int>> destroyedShips; // group index, ship indices.

	// Try to destroy ships (biggest type and most damaged ship first.) 
	std::vector<int> groupIndices = GetTargetGroupIndicesBiggestFirst();
	for (int groupIndex : groupIndices)
	{
		if (remainingDice.empty())
			break;

		const Group& group = m_groups[groupIndex];
		int toHit = GetToHitRoll(group.shipType, game);

		Hits groupHits = GetHitsToDestroy(group, remainingDice, toHit);
		hits.insert(hits.begin(), groupHits.begin(), groupHits.end());

		// Remember destroyed ships so we don't try to damage them. 
		for (auto& hit : groupHits)
			destroyedShips[groupIndex].insert(hit.shipIndex);
	}

	// Try to damage remaining ships. Only need to consider the weakest ship of each group. 
	for (int groupIndex : groupIndices)
	{
		if (remainingDice.empty())
			break;

		const Group& group = m_groups[groupIndex];
		const auto shipIndices = GetShipIndicesWeakestFirst(group);

		// Check if the whole group has been destroyed. 
		// We can't just check if shipIndices is empty, because the damamge hasn't been applied yet. 
		if (destroyedShips[groupIndex].size() == shipIndices.size())
			continue;

		int toHit = GetToHitRoll(group.shipType, game);

		for (int shipIndex : shipIndices)
		{
			if (destroyedShips[groupIndex].count(shipIndex) == 0) // Still alive.
			{
				Dice used = remainingDice.RemoveAll(toHit);
				if (!used.empty())
					hits.push_back(Hit(group.shipType, shipIndex, used));

				break; // No point checking healthier ships. 
			}
		}
	}

	return hits;
}

Battle::Group* ShipBattle::FindTargetGroup(ShipType shipType)
{
	for (Group& group : m_groups)
		if (group.invader != GetCurrentGroup().invader && group.shipType == shipType)
			return &group;
	return nullptr;
}

RecordPtr ShipBattle::CreateAttackRecord(const Game& game, const Dice& dice) const
{
	const Hits hits = AutoAssignHits(dice, game);
	return RecordPtr(new AttackShipsRecord(hits));
}
REGISTER_DYNAMIC(ShipBattle)
