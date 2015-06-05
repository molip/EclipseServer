#include "stdafx.h"
#include "ReputationRecord.h"

#include "Game.h"
#include "GameState.h"
#include "Output.h"
#include "TeamState.h"

ReputationRecord::ReputationRecord()
{
}

ReputationRecord::ReputationRecord(const TileValues& values) 
{
	for (auto& pair : values)
		m_values[pair.first] = std::make_pair(pair.second, 0);
}

ReputationRecord::~ReputationRecord()
{
}

std::string ReputationRecord::GetMessage(const Game & game) const 
{
	return "Assigned reputation";
}

void ReputationRecord::Apply(bool bDo, const Game & game, GameState & gameState)
{
	for (auto& pair : m_values)
	{
		auto& valUndoPair = pair.second;
		auto& repTrack = gameState.GetTeamState(pair.first).GetReputationTrack();
		if (bDo)
		{
			gameState.GetReputationBag().TakeTile(valUndoPair.first);
			valUndoPair.second = repTrack.AddReputationTile(valUndoPair.first);
		}
		else
		{
			repTrack.RemoveReputationTile(valUndoPair.first);
			if (valUndoPair.second)
				repTrack.AddReputationTile(valUndoPair.second);
			gameState.GetReputationBag().ReturnTile(valUndoPair.first);
		}
	}
}

void ReputationRecord::Update(const Game& game, const RecordContext& context) const
{
	for (auto& team : game.GetTeams())
		for (auto& pair : m_values)
			context.SendMessage(Output::UpdateReputationTrack(game.GetTeam(pair.first), pair.first == team->GetColour()));
}

void ReputationRecord::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveMap("values", m_values, Serial::EnumSaver(), Serial::PairSaver<Serial::TypeSaver, Serial::TypeSaver>());
}

void ReputationRecord::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadMap("values", m_values, Serial::EnumLoader(), Serial::PairLoader<Serial::TypeLoader, Serial::TypeLoader>());
}
