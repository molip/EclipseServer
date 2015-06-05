#include "stdafx.h"
#include "Battle.h"
#include "App.h"
#include "Hex.h"
#include "Blueprint.h"
#include "LiveGame.h"
#include "Test.h"
#include "Record.h"

Battle::Group::Group() : shipType(ShipType::None), invader(false), hasMissiles(false) 
{
}

Battle::Group::Group(ShipType _shipType, bool _invader) : 
	shipType(_shipType), invader(_invader), hasMissiles(false)
{
}

bool Battle::Group::operator==(const Group& rhs) const
{
	if (shipType == rhs.shipType &&
		invader == rhs.invader &&
		hasMissiles == rhs.hasMissiles)
		return true;

	return false;
}

bool Battle::Group::IsDead() const
{
	for (int lives : lifeCounts)
		if (lives > 0)
			return false;
	return true;
}

int Battle::Group::GetDeadShipCount() const
{
	return (int)std::count_if(lifeCounts.begin(), lifeCounts.end(), [](int c) { return c == 0; });
}

void Battle::Group::Save(Serial::SaveNode& node) const
{
	using namespace Serial;

	node.SaveEnum("ship_type", shipType);
	node.SaveType("invader", invader);
	node.SaveType("has_missiles", hasMissiles);
	node.SaveCntr("life_counts", lifeCounts, TypeSaver());
}

void Battle::Group::Load(const Serial::LoadNode& node)
{
	using namespace Serial;

	node.LoadEnum("ship_type", shipType);
	node.LoadType("invader", invader);
	node.LoadType("has_missiles", hasMissiles);
	node.LoadCntr("life_counts", lifeCounts, TypeLoader());
}

//-----------------------------------------------------------------------------

void Battle::Turn::Save(Serial::SaveNode& node) const
{
	node.SaveType("group_index", groupIndex);
	node.SaveEnum("phase", phase);
}

void Battle::Turn::Load(const Serial::LoadNode& node)
{
	node.LoadType("group_index", groupIndex);
	node.LoadEnum("phase", phase);
}

//-----------------------------------------------------------------------------

Battle::Battle() : m_hexId(0)
{
}

Battle::Battle(const Hex& hex, const Game& game, const GroupVec& oldGroups) : m_hexId(hex.GetID())
{
	VERIFY_MODEL(hex.GetPendingBattle(m_defender, m_invader, game));

	auto pred = [&](const Group& lhs, const Group& rhs)
	{
		int lhsInit = GetBlueprint(game, lhs.shipType, lhs.invader).GetInitiative();
		int rhsInit = GetBlueprint(game, rhs.shipType, rhs.invader).GetInitiative();

		if (lhsInit == rhsInit)
			return lhs.invader < rhs.invader; // Defender goes first.

		return lhsInit > rhsInit;
	};

	AddGroups(false, hex, game);

	if (oldGroups.empty())
		AddGroups(true, hex, game);
	else
		AddOldGroups(oldGroups, hex, game);

	// Sort groups by initiative.
	std::sort(m_groups.begin(), m_groups.end(), pred);
}

bool Battle::operator==(const Battle& rhs) const
{
	if (m_defender == rhs.m_defender &&
		m_invader == rhs.m_invader &&
		m_hexId == rhs.m_hexId &&
		m_groups == rhs.m_groups &&
		m_turn == rhs.m_turn)
		return true;

	return false;
}

void Battle::AddGroups(bool invader, const Hex& hex, const Game& game)
{
	for (auto shipType : AllShipTypesRange())
		if (int count = hex.GetShipCount(GetColour(!!invader), shipType))
		{
			m_groups.push_back(Group(shipType, invader));
			m_groups.back().lifeCounts.resize(count, GetBlueprint(game, shipType, invader).GetLives());
		}
}

void Battle::AddOldGroups(const GroupVec& oldGroups, const Hex& hex, const Game& game)
{
	for (auto& oldGroup : oldGroups)
	{
		std::vector<int> lifeCounts;
		for (int lives : oldGroup.lifeCounts)
			if (lives)
				lifeCounts.push_back(lives);

		if (!lifeCounts.empty())
		{
			VERIFY_MODEL(lifeCounts.size() == hex.GetShipCount(GetColour(true), oldGroup.shipType));
			m_groups.push_back(Group(oldGroup.shipType, true));
			m_groups.back().lifeCounts = lifeCounts;
		}
	}
}

int Battle::FindFirstMissileGroup() const
{
	for (int i = 0; i < (int)m_groups.size(); ++i)
		if (m_groups[i].hasMissiles && !m_groups[i].IsDead())
			return i;
	return -1;
}

bool Battle::IsMainPhaseFinished() const
{
	bool alive[2] = {};
	for (auto& group : m_groups)
	{
		alive[group.invader] |= !group.IsDead();
		if (alive[0] && alive[1])
			return false;
	}
	return true;
}

Battle::Turn Battle::AdvanceTurn(const Game& game)
{
	if (IsFinished())
	{
		VERIFY_MODEL(false);
		return m_turn;
	}
	Turn oldTurn = m_turn;
	DoAdvanceTurn(game);
	return oldTurn;
}

void Battle::SetTurn(const Turn& turn)
{
	m_turn = turn;
}

const Blueprint& Battle::GetBlueprint(const Game& game, const Group& group) const
{
	return GetBlueprint(game, group.shipType, group.invader);
}

const Blueprint& Battle::GetBlueprint(const Game& game, ShipType shipType, bool invader) const
{
	return Ship::GetBlueprint(GetColour(invader), shipType, game);
}

const Blueprint& Battle::GetCurrentBlueprint(const Game& game) const
{
	const Group& group = GetCurrentGroup();
	return GetBlueprint(game, group.shipType, group.invader);
}

void Battle::RollDice(const LiveGame& game, Dice& dice) const
{
	ASSERT(dice.empty());

	const Blueprint& blueprint = GetCurrentBlueprint(game);

	for (int lives : GetCurrentGroup().lifeCounts)
		if (lives > 0)
			blueprint.AddDice(dice, IsMissilePhase());
}

int Battle::GetToHitRoll(ShipType shipType, const Game& game) const
{
	const Blueprint& firing = GetBlueprint(game, GetCurrentGroup());
	const Blueprint& target = GetBlueprint(game, shipType, !GetCurrentGroup().invader);
	return Dice::GetToHitRoll(firing.GetComputer(), target.GetShield());
}

RecordPtr Battle::CreateAutoAttackRecord(const Game& game) const 
{ 
	return nullptr;
}

void Battle::AddReputationResults(ReputationResults& results) const
{
	VERIFY_MODEL(IsFinished());
		
	for (auto& group : m_groups)
	{
		Colour colour = GetColour(!group.invader);
		if (colour != Colour::None)
		{
			int& count = results[colour]; // Assign rep tiles to the other team. 
			count += ::GetShipTypeReputationTileCount(group.shipType) * group.GetDeadShipCount();
		}
	}

	// TODO: Only do this if not retreated. 
	if (m_defender != Colour::None)
		++results[m_defender];
	if (m_invader != Colour::None)
		++results[m_invader];
}

void Battle::Save(Serial::SaveNode& node) const
{
	using namespace Serial;

	node.SaveEnum("defender", m_defender);
	node.SaveEnum("invader", m_invader);
	node.SaveCntr("groups", m_groups, ClassSaver());
	node.SaveClass("turn", m_turn);
	node.SaveType("hex_id", m_hexId);
}

void Battle::Load(const Serial::LoadNode& node)
{
	using namespace Serial;

	node.LoadEnum("defender", m_defender);
	node.LoadEnum("invader", m_invader);
	node.LoadCntr("groups", m_groups, ClassLoader());
	node.LoadClass("turn", m_turn);
	node.LoadType("hex_id", m_hexId);
}

DEFINE_ENUM_NAMES(BattlePhase) { "Missile", "Main", "Population", "" };
