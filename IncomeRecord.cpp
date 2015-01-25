#include "stdafx.h"
#include "IncomeRecord.h"
#include "Output.h"
#include "Controller.h"
#include "Player.h"
#include "LiveGame.h"

IncomeRecord::IncomeRecord(Colour colour) : TeamRecord(colour)
{
}

void IncomeRecord::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveClass("income", m_income);
}
	
void IncomeRecord::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadClass("income", m_income);
}

void IncomeRecord::Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState)
{
	if (bDo)
		teamState.GetStorage() += team.GetIncome();
	else
		teamState.GetStorage() -= team.GetIncome();
}

void IncomeRecord::Update(const Game& game, const Team& team, const RecordContext& context) const
{
	context.SendMessage(Output::UpdateStorageTrack(team));
}

std::string IncomeRecord::GetTeamMessage() const
{
	return FormatString("received income");
}

REGISTER_DYNAMIC(IncomeRecord)

