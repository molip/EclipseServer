#include "Battle.h"
#include "App.h"
#include "Hex.h"
#include "Blueprint.h"
#include "LiveGame.h"
#include "Test.h"

Battle::Hit::Hit() : shipType(ShipType::None), shipIndex(-1) {}
Battle::Hit::Hit(ShipType _shipType, int _shipIndex, const Dice& _dice) : 
	shipType(_shipType), shipIndex(_shipIndex), dice(_dice) {}

void Battle::Hit::Save(Serial::SaveNode& node) const
{
	node.SaveEnum("ship_type", shipType);
	node.SaveType("ship_index", shipIndex);
	node.SaveClass("dice", dice);
}

void Battle::Hit::Load(const Serial::LoadNode& node)
{
	node.LoadEnum("ship_type", shipType);
	node.LoadType("ship_index", shipIndex);
	node.LoadClass("dice", dice);
}

void Battle::Hits::Save(Serial::SaveNode& node) const
{
	node.SaveCntr("hits", *this, Serial::ClassSaver());
}

void Battle::Hits::Load(const Serial::LoadNode& node)
{
	node.LoadCntr("hits", *this, Serial::ClassLoader());
}

void Battle::PopulationHits::Save(Serial::SaveNode& node) const
{
	node.SaveCntr("hits", *this, Serial::TypeSaver());
	node.SaveType("auto_hit", autoHit);
}

void Battle::PopulationHits::Load(const Serial::LoadNode& node)
{
	node.LoadCntr("hits", *this, Serial::TypeLoader());
	node.LoadType("auto_hit", autoHit);
}

//-----------------------------------------------------------------------------

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

	VERIFY_MODEL(m_groups.size() >= 2);

	bool missiles = false;
	for (auto& group : m_groups)
	{
		group.hasMissiles = GetBlueprint(game, group.shipType, group.invader).HasMissiles();
		missiles |= group.hasMissiles;
	}

	// Sort groups by initiative.
	std::sort(m_groups.begin(), m_groups.end(), pred);

	m_turn.groupIndex = missiles ? FindFirstMissileGroup() : 0;
	m_turn.phase = missiles ? BattlePhase::Missile : BattlePhase::Main;
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
	for (size_t i = 0; i < m_groups.size(); ++i)
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
			m_turn.groupIndex = -1; // Finished. Unless...
			if (hex->CanAttackPopulation() && !CanAutoDestroyPopulation(game))
				m_turn.phase = BattlePhase::Population; // m_turn.groupIndex gets advanced below. 
		}
		else
		{
			do
				m_turn.groupIndex = (m_turn.groupIndex + 1) % m_groups.size();
			while (GetCurrentGroup().IsDead()); // TODO: Check cannons. 
		}
	}

	if (m_turn.phase == BattlePhase::Population)
	{
		if (hex->CanAttackPopulation())
		{
			while (++m_turn.groupIndex < (int)m_groups.size() && GetCurrentGroup().IsDead()); // TODO: Check cannons. 
			if (m_turn.groupIndex == m_groups.size()) 
				m_turn.groupIndex = -1; // All groups had a go - finished.
		}
		else
			m_turn.groupIndex = -1; // All population dead - finished.
	}

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

std::vector<int> Battle::GetShipIndicesWeakestFirst(const Group& group) const
{
	auto pred = [&](int lhs, int rhs) { return group.lifeCounts[lhs] < group.lifeCounts[rhs]; };

	std::vector<int> indices;
	for (size_t i = 0; i < group.lifeCounts.size(); ++i)
		if (group.lifeCounts[i] > 0) // Ignore dead ones.
			indices.push_back(i);

	std::sort(indices.begin(), indices.end(), pred);
	return indices;
}

std::vector<int> Battle::GetTargetGroupIndicesBiggestFirst() const
{
	auto pred = [&](int lhs, int rhs) { return GetShipTypeSize(m_groups[lhs].shipType) > GetShipTypeSize(m_groups[rhs].shipType); };

	std::vector<int> indices;
	for (size_t i = 0; i < m_groups.size(); ++i)
		if (m_groups[i].invader != GetCurrentGroup().invader)
			indices.push_back(i);
	std::sort(indices.begin(), indices.end(), pred);
	return indices;
}

Battle::Hits Battle::GetHitsToDestroy(const Group& group, Dice& dice, int toHit) const
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

Battle::Hits Battle::AutoAssignHits(const Dice& dice, const Game& game) const
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

Battle::PopulationHits Battle::AutoAssignPopulationHits(const Dice& dice, const Game& game) const
{
	Battle::PopulationHits hits;

	const Blueprint& firing = GetBlueprint(game, GetCurrentGroup());
	int toHit = Dice::GetToHitRoll(firing.GetComputer(), 0);
	int damage = dice.GetDamage(toHit);

	auto& squares = game.GetMap().FindHex(m_hexId)->GetSquares();
	for (size_t i = 0; i < squares.size() && damage > 0; ++i)
		if (squares[i].IsOccupied())
		{
			hits.push_back(i);
			--damage;
		}

	return hits;
}

Battle::Group* Battle::FindTargetGroup(ShipType shipType)
{
	for (Group& group : m_groups)
		if (group.invader != GetCurrentGroup().invader && group.shipType == shipType)
			return &group;
	return nullptr;
}

bool Battle::CanAutoDestroyPopulation(const Game& game) const
{
	const Hex* hex = game.GetMap().FindHex(m_hexId);
	return hex->CanAttackPopulation() && game.GetTeam(hex->GetFleets().front().GetColour()).HasTech(TechType::NeutronBomb);
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
