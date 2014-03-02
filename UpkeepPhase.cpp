#include "UpkeepPhase.h"
#include "LiveGame.h"
#include "CmdStack.h"
#include "Cmd.h"
#include "Controller.h"
#include "Output.h"
#include "Player.h"
#include "StartRoundRecord.h"

UpkeepPhase::UpkeepPhase(LiveGame* pGame) : Phase(pGame)
{
	if (pGame)
		for (auto& team : pGame->GetTeams())
		{
			VerifyModel("UpkeepPhase::UpkeepPhase: Team has no colour", team->GetColour() != Colour::None);
			auto pair = m_cmdStacks.insert(std::make_pair(team->GetColour(), CmdStackPtr(new CmdStack)));
			VerifyModel("LiveGame::AssignTeam", pair.second);
		}
}

CmdStack& UpkeepPhase::GetCmdStack(Colour c)
{
	auto it = m_cmdStacks.find(c);
	VerifyModel("Phase::GetCmdStack", it != m_cmdStacks.end());
	return *it->second;
}

void UpkeepPhase::StartCmd(CmdPtr pCmd, Controller& controller)
{
	VerifyModel("UpkeepPhase::StartCmd", !pCmd->IsAction());

	Colour c = pCmd->GetColour();
	GetCmdStack(c).StartCmd(pCmd);

	SaveGame();

	GetCurrentCmd(c)->UpdateClient(controller, GetGame());
}

void UpkeepPhase::AddCmd(CmdPtr pCmd)
{
	VerifyModel("Phase::UpkeepPhase", (bool)pCmd);

	GetCmdStack(pCmd->GetColour()).AddCmd(pCmd);
	SaveGame();
}

void UpkeepPhase::FinishCmd(Colour c)
{
	GetCmdStack(c).AddCmd(CmdPtr());
	SaveGame();
}

Cmd* UpkeepPhase::RemoveCmd(const Controller& controller, Colour c)
{
	const Cmd* pCmd = GetCurrentCmd(c);

	Cmd* pUndo = GetCmdStack(c).RemoveCmd();

	SaveGame();
	return pUndo;
}

bool UpkeepPhase::CanRemoveCmd(Colour c) const
{
	return GetCmdStack(c).CanRemoveCmd();
}

bool UpkeepPhase::IsTeamActive(Colour c) const 
{
	return m_cmdStacks.find(c) != m_cmdStacks.end();
}

Cmd* UpkeepPhase::GetCurrentCmd(Colour c)
{
	return GetCmdStack(c).GetCurrentCmd();
}

void UpkeepPhase::UpdateClient(const Controller& controller, const Player* pPlayer) const
{
	for (auto& team : GetGame().GetTeams())
	{
		const Player& playerSend = team->GetPlayer();
		if (!pPlayer || pPlayer == &playerSend)
		{
			const Colour c = team->GetColour();
			const CmdStack& cmdStack = GetCmdStack(c);
			if (const Cmd* pCmd = cmdStack.GetCurrentCmd())
				pCmd->UpdateClient(controller, GetGame());
			else if (m_finished.find(c) == m_finished.end())
				controller.SendMessage(Output::ChooseUpkeep(*team, cmdStack.CanRemoveCmd()), playerSend);
			else
				controller.SendMessage(Output::ChooseFinished(), playerSend);
		}
	}
}

void UpkeepPhase::FinishTurn(Controller& controller, const Player& player)
{
	LiveGame& game = GetGame();
	
	const Colour c = game.GetTeam(player).GetColour();

	VerifyModel("UpkeepPhase::FinishTurn", GetCurrentCmd(c) == nullptr);

	bool bOK = m_finished.insert(c).second;
	VerifyModel("UpkeepPhase::FinishTurn", bOK);

	UpdateClient(controller, &player);

	if (m_finished.size() == game.GetTeams().size())
	{
		Record::DoAndPush(RecordPtr(new StartRoundRecord), game, controller);

		game.StartActionPhase(); // Deletes this.
		game.GetPhase().UpdateClient(controller, nullptr);
	}
}

void UpkeepPhase::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveMap("command_stacks", m_cmdStacks, Serial::EnumSaver(), Serial::ClassPtrSaver());
	node.SaveCntr("finished", m_finished, Serial::EnumSaver());
}

void UpkeepPhase::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadMap("command_stacks", m_cmdStacks, Serial::EnumLoader(), Serial::ClassPtrLoader());
	node.LoadCntr("finished", m_finished, Serial::EnumLoader());
}

REGISTER_DYNAMIC(UpkeepPhase)
