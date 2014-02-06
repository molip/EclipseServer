#include "stdafx.h"
#include "StartRoundRecord.h"
#include "Output.h"
#include "Controller.h"
#include "Player.h"
#include "LiveGame.h"

void StartRoundRecord::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveCntr("techs", m_techs, Serial::EnumSaver());
}
	
void StartRoundRecord::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadCntr("techs", m_techs, Serial::EnumLoader());
}

void StartRoundRecord::Apply(bool bDo, Game& game, const Controller& controller) 
{
	game.IncrementRound(bDo);

	// Take new technologies from bag.

	std::map<TechType, int>& supplyTechs = game.GetTechnologies();
	TechnologyBag& techBag = game.GetTechnologyBag();
	
	if (bDo)
	{
		const int startTech[] = { 12, 12, 14, 16, 18, 20 };
		const int roundTech[] = { 4, 4, 6, 7, 8, 9 };

		int nTech = (game.GetRound() == 0 ? startTech : roundTech)[game.GetTeams().size() - 1];

		for (int i = 0; i < nTech && !techBag.IsEmpty(); ++i)
		{
			m_techs.push_back(techBag.TakeTile());
			++supplyTechs[m_techs.back()];
		}
	}
	else
	{
		for (auto i = m_techs.rbegin(); i != m_techs.rend(); ++i)
		{
			techBag.ReturnTile(*i);
			--supplyTechs[*i];
		}
		m_techs.clear();
	}
	
	for (auto& team : game.GetTeams())
	{
		team->SetPassed(!bDo);
		controller.SendMessage(Output::UpdatePassed(*team), game);
	}

	// TODO: reset colony ships.

	controller.SendMessage(Output::UpdateRound(game), game);
	controller.SendMessage(Output::UpdateTechnologies(game), game);
}

REGISTER_DYNAMIC(StartRoundRecord)

