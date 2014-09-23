#include "Battle.h"
#include "App.h"
#include "Hex.h"
#include "Blueprint.h"
#include "LiveGame.h"
#include "Dice.h"
#include "Test.h"

Battle::Battle() : m_groupIndex(0), m_missilePhase(false)
{
}

Battle::Battle(const Hex& hex, const Game& game)
{
	VerifyModel("Battle::Battle 1", hex.GetPendingBattle(m_defender, m_invader, game));

	auto pred = [&](const Group& lhs, const Group& rhs)
	{
		int lhsInit = GetBlueprint(game, lhs.shipType, lhs.invader).GetInitiative();
		int rhsInit = GetBlueprint(game, rhs.shipType, rhs.invader).GetInitiative();

		if (lhsInit == rhsInit)
			return lhs.invader < rhs.invader; // Defender goes first.

		return lhsInit > rhsInit;
	};

	for (int invader = 0; invader < 2; ++invader)
		for (auto shipType : EnumRange<ShipType>(ShipType::_First))
			if (int count = hex.GetShipCount(GetColour(!!invader), shipType))
			{
				bool missiles = GetBlueprint(game, shipType, !!invader).HasMissiles();
				m_groups.push_back(Group{ shipType, !!invader, missiles });
				m_groups.back().ships.resize(count);

				m_missilePhase |= missiles;
			}

	VerifyModel("Battle::Battle 2", m_groups.size() >= 2);

	// Sort groups by initiative.
	std::sort(m_groups.begin(), m_groups.end(), pred);

	m_groupIndex = m_missilePhase ? FindFirstMissileGroup() : 0;
}

int Battle::FindFirstMissileGroup() const
{
	for (size_t i = 0; i < m_groups.size(); ++i)
		if (m_groups[i].hasMissiles)
			return i;
	return -1;
}

void Battle::AdvanceTurn()
{
	if (m_missilePhase)
	{
		VerifyModel("Battle::AdvanceTurn", GetCurrentGroup().hasMissiles);
		GetCurrentGroup().hasMissiles = false;
		m_groupIndex = std::max(FindFirstMissileGroup(), 0);
	}
	else
		m_groupIndex = (m_groupIndex + 1) % m_groups.size();
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

	for (size_t i = 0; i < GetCurrentGroup().ships.size(); ++i)
		blueprint.AddDice(dice, m_missilePhase);
}

void Battle::Save(Serial::SaveNode& node) const
{
	using namespace Serial;

	node.SaveEnum("defender", m_defender);
	node.SaveEnum("invader", m_invader);
	node.SaveCntr("groups", m_groups, ClassSaver());
	node.SaveType("turn", m_groupIndex);
	node.SaveType("missile_phase", m_missilePhase);
}

void Battle::Load(const Serial::LoadNode& node)
{
	using namespace Serial;

	node.LoadEnum("defender", m_defender);
	node.LoadEnum("invader", m_invader);
	node.LoadCntr("groups", m_groups, ClassLoader());
	node.LoadType("turn", m_groupIndex);
	node.LoadType("missile_phase", m_missilePhase);
}

//-----------------------------------------------------------------------------

void Battle::Group::Save(Serial::SaveNode& node) const
{
	using namespace Serial;

	node.SaveEnum("ship_type", shipType);
	node.SaveType("invader", invader);
	node.SaveType("has_missiles", hasMissiles);
	node.SaveCntr("ships", ships, TypeSaver());
}

void Battle::Group::Load(const Serial::LoadNode& node)
{
	using namespace Serial;

	node.LoadEnum("ship_type", shipType);
	node.LoadType("invader", invader);
	node.LoadType("has_missiles", hasMissiles);
	node.LoadCntr("ships", ships, TypeLoader());
}
