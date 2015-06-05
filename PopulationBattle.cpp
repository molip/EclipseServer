#include "stdafx.h"
#include "PopulationBattle.h"
#include "App.h"
#include "Hex.h"
#include "Blueprint.h"
#include "LiveGame.h"
#include "Test.h"
#include "AttackPopulationRecord.h"

void PopulationBattle::Hits::Save(Serial::SaveNode& node) const
{
	node.SaveCntr("hits", *this, Serial::TypeSaver());
	node.SaveType("auto_hit", autoHit);
}

void PopulationBattle::Hits::Load(const Serial::LoadNode& node)
{
	node.LoadCntr("hits", *this, Serial::TypeLoader());
	node.LoadType("auto_hit", autoHit);
}

//-------------------------------------------------------------------

PopulationBattle::PopulationBattle()
{
}

PopulationBattle::PopulationBattle(const Hex& hex, const Game& game, const GroupVec& oldGroups) : Battle(hex, game, oldGroups)
{
	m_turn.phase = BattlePhase::Population;
}

bool PopulationBattle::operator==(const Battle& rhs) const
{
	return dynamic_cast<const PopulationBattle*>(&rhs) && __super::operator==(rhs);
}

BattlePtr PopulationBattle::Clone() const
{
	return BattlePtr(new PopulationBattle(*this));
}

void PopulationBattle::DoAdvanceTurn(const Game& game)
{
	VERIFY(m_turn.phase == BattlePhase::Population);

	const Hex* hex = game.GetMap().FindHex(m_hexId);
	if (hex->CanAttackPopulation())
	{
		while (++m_turn.groupIndex < (int)m_groups.size() && GetCurrentGroup().IsDead()); // TODO: Check cannons. 
		if (m_turn.groupIndex == m_groups.size())
			m_turn.groupIndex = -1; // All groups had a go - finished.
	}
	else
		m_turn.groupIndex = -1; // All population dead - finished.
}

PopulationBattle::Hits PopulationBattle::AutoAssignPopulationHits(const Dice& dice, const Game& game) const
{
	PopulationBattle::Hits hits;

	const Blueprint& firing = GetBlueprint(game, GetCurrentGroup());
	int toHit = Dice::GetToHitRoll(firing.GetComputer(), 0);
	int damage = dice.GetDamage(toHit);

	auto& squares = game.GetMap().FindHex(m_hexId)->GetSquares();
	for (int i = 0; i < (int)squares.size() && damage > 0; ++i)
		if (squares[i].IsOccupied())
		{
			hits.push_back(i);
			--damage;
		}

	return hits;
}

bool PopulationBattle::CanAutoDestroyPopulation(const Game& game) const
{
	const Hex* hex = game.GetMap().FindHex(m_hexId);
	return hex->CanAttackPopulation() && game.GetTeam(hex->GetFleets().front().GetColour()).HasTech(TechType::NeutronBomb);
}

RecordPtr PopulationBattle::CreateAttackRecord(const Game& game, const Dice& dice) const
{
	const Hits hits = game.GetPopulationBattle().AutoAssignPopulationHits(dice, game);
	return RecordPtr(new AttackPopulationRecord(hits));
}

RecordPtr PopulationBattle::CreateAutoAttackRecord(const Game& game) const
{
	if (CanAutoDestroyPopulation(game))
		return RecordPtr(new AttackPopulationRecord(PopulationBattle::Hits(true)));
	return nullptr;
}

REGISTER_DYNAMIC(PopulationBattle)
