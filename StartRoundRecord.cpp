#include "stdafx.h"
#include "StartRoundRecord.h"
#include "Output.h"
#include "Controller.h"
#include "Player.h"
#include "LiveGame.h"

StartRoundRecord::StartRoundRecord() : m_round(0)
{
}

void StartRoundRecord::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveCntr("techs", m_techs, Serial::EnumSaver());
	node.SaveCntr("team_data", m_teamData, Serial::ClassSaver());
	node.SaveType("round", m_round);
}
	
void StartRoundRecord::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadCntr("techs", m_techs, Serial::EnumLoader());
	node.LoadCntr("team_data", m_teamData, Serial::ClassLoader());
	node.LoadType("round", m_round);
}

void StartRoundRecord::Apply(bool bDo, const RecordContext& context) 
{
	Game& game = context.GetGame();
	GameState& gameState = context.GetGameState();

	bool bFirstRound = false;

	gameState.IncrementRound(bDo);

	// Take new technologies from bag.

	std::map<TechType, int>& supplyTechs = gameState.GetTechnologies();
	TechnologyBag& techBag = game.GetTechnologyBag();
	
	if (bDo)
	{
		bFirstRound = game.GetRound() == 0;
		m_round = game.GetRound();

		if (!bFirstRound)
		{
			for (auto& team : game.GetTeams())
			{
				TeamData data{ team->GetActionTrack().GetDiscCount(), team->GetUsedColonyShips(), team->GetIncome() };
				m_teamData.push_back(data);

				TeamState& teamState = gameState.GetTeamState(team->GetColour());
				teamState.GetActionTrack().RemoveDiscs(data.actions);
				teamState.GetInfluenceTrack().AddDiscs(data.actions);
				teamState.ReturnColonyShips(data.colonyShips);
				teamState.GetStorage() += data.income;
			}
		}
		
		const int startTech[] = { 12, 12, 14, 16, 18, 20 };
		const int roundTech[] = { 4, 4, 6, 7, 8, 9 };

		int nTech = (bFirstRound ? startTech : roundTech)[game.GetTeams().size() - 1];

		for (int i = 0; i < nTech && !techBag.IsEmpty(); ++i)
		{
			m_techs.push_back(techBag.TakeTile());
			++supplyTechs[m_techs.back()];
		}
	}
	else
	{
		bFirstRound = game.GetRound() < 0;
		if (!bFirstRound)
		{
			ASSERT(m_teamData.size() == game.GetTeams().size());

			for (size_t i = 0; i < m_teamData.size(); ++i)
			{
				const TeamData& data = m_teamData[i];
				Team& team = *game.GetTeams()[i];
				TeamState& teamState = gameState.GetTeamState(team.GetColour());
				teamState.GetActionTrack().AddDiscs(data.actions);
				teamState.GetInfluenceTrack().RemoveDiscs(data.actions);
				teamState.UseColonyShips(data.colonyShips);
				teamState.GetStorage() -= data.income;
			}
			m_teamData.clear();
		}

		for (auto i = m_techs.rbegin(); i != m_techs.rend(); ++i)
		{
			techBag.ReturnTile(*i);
			--supplyTechs[*i];
		}
		m_techs.clear();
	}
	
	if (!bFirstRound)
	{
		for (auto& team : game.GetTeams())
		{
			TeamState& teamState = gameState.GetTeamState(team->GetColour());
			teamState.SetPassed(!bDo);
			context.SendMessage(Output::UpdatePassed(*team));
			context.SendMessage(Output::UpdateInfluenceTrack(*team));
			context.SendMessage(Output::UpdateActionTrack(*team));
			context.SendMessage(Output::UpdateColonyShips(*team));
			context.SendMessage(Output::UpdateStorageTrack(*team));
		}
	}
	// TODO: reset colony ships.

	context.SendMessage(Output::UpdateRound(game));
	context.SendMessage(Output::UpdateTechnologies(game));
}

std::string StartRoundRecord::GetMessage(const Game& game) const
{
	return FormatString("Starting round %0", m_round + 1);
}

void StartRoundRecord::TeamData::Save(Serial::SaveNode& node) const
{
	node.SaveType("actions", actions);
	node.SaveType("colony_ships", colonyShips);
	node.SaveClass("income", income);
}

void StartRoundRecord::TeamData::Load(const Serial::LoadNode& node)
{
	node.LoadType("actions", actions);
	node.LoadType("colony_ships", colonyShips);
	node.LoadClass("income", income);
}

REGISTER_DYNAMIC(StartRoundRecord)

