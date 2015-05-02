#include "stdafx.h"
#include "StartRoundRecord.h"
#include "Output.h"
#include "Controller.h"
#include "Player.h"
#include "LiveGame.h"

StartRoundRecord::StartRoundRecord() : m_round(-1), m_techCount(0)
{
}

void StartRoundRecord::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveMap("team_data", m_teamData, Serial::EnumSaver(), Serial::ClassSaver());
	node.SaveType("round", m_round);
	node.SaveType("tech_count", m_techCount);
}
	
void StartRoundRecord::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadMap("team_data", m_teamData, Serial::EnumLoader(), Serial::ClassLoader());
	node.LoadType("round", m_round);
	node.LoadType("tech_count", m_techCount);
}

void StartRoundRecord::Apply(bool bDo, const Game& game, GameState& gameState) 
{
	if (gameState.IncrementRound(bDo)) // Finished.
		return;

	if (bDo && m_round < 0) // First time.
	{
		m_round = game.GetRound();
		VERIFY(m_round >= 1); // First round uses StartGameRecord instead.

		for (auto& team : game.GetTeams())
		{
			TeamData data{ team->GetActionTrack().GetDiscCount(), team->GetUsedColonyShips() };
			m_teamData.insert(std::make_pair(team->GetColour(), data));
		}

		const int roundTech[] = { 4, 4, 6, 7, 8, 9 };
		m_techCount = std::min(roundTech[game.GetTeams().size() - 1], gameState.GetTechnologyBag().GetRemaining());
	}

	for (auto& team : game.GetTeams())
	{
		const TeamData& data = m_teamData[team->GetColour()];
		TeamState& teamState = gameState.GetTeamState(team->GetColour());

		if (bDo)
		{
			teamState.GetActionTrack().RemoveDiscs(data.actions);
			teamState.GetInfluenceTrack().AddDiscs(data.actions);
			teamState.ReturnColonyShips(data.colonyShips);
		}
		else
		{
			teamState.GetActionTrack().AddDiscs(data.actions);
			teamState.GetInfluenceTrack().RemoveDiscs(data.actions);
			teamState.UseColonyShips(data.colonyShips);
		}

		teamState.SetPassed(!bDo);
	}

	gameState.ProduceTechnologies(m_techCount, bDo); 
}

void StartRoundRecord::Update(const Game& game, const RecordContext& context) const
{
	if (!m_teamData.empty())
		for (auto& team : game.GetTeams())
		{
			context.SendMessage(Output::UpdatePassed(*team));
			context.SendMessage(Output::UpdateInfluenceTrack(*team));
			context.SendMessage(Output::UpdateActionTrack(*team));
			context.SendMessage(Output::UpdateColonyShips(*team));
			context.SendMessage(Output::UpdateStorageTrack(*team));
		}

	context.SendMessage(Output::UpdateRound(game));
	context.SendMessage(Output::UpdateTechnologies(game));
	context.SendMessage(Output::UpdateScore(game, game.HasFinished()));
}

std::string StartRoundRecord::GetMessage(const Game& game) const
{
	return FormatString("Starting round %0. %1 techs produced.", m_round + 1, m_techCount);
}

void StartRoundRecord::TeamData::Save(Serial::SaveNode& node) const
{
	node.SaveType("actions", actions);
	node.SaveType("colony_ships", colonyShips);
}

void StartRoundRecord::TeamData::Load(const Serial::LoadNode& node)
{
	node.LoadType("actions", actions);
	node.LoadType("colony_ships", colonyShips);
}

REGISTER_DYNAMIC(StartRoundRecord)

