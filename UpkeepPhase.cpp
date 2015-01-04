#include "UpkeepPhase.h"
#include "LiveGame.h"
#include "CmdStack.h"
#include "Cmd.h"
#include "Controller.h"
#include "Output.h"
#include "Player.h"
#include "StartRoundRecord.h"
#include "CommitSession.h"

UpkeepPhase::UpkeepPhase(LiveGame* pGame) : Phase(pGame)
{
	if (pGame)
		for (auto& team : pGame->GetTeams())
		{
			VERIFY_MODEL_MSG("Team has no colour", team->GetColour() != Colour::None);
			auto pair = m_cmdStacks.insert(std::make_pair(team->GetColour(), CmdStackPtr(new CmdStack)));
			VERIFY_MODEL(pair.second);
		}
}

CmdStack& UpkeepPhase::GetCmdStack(Colour c)
{
	auto it = m_cmdStacks.find(c);
	VERIFY_MODEL(it != m_cmdStacks.end());
	return *it->second;
}

void UpkeepPhase::StartCmd(CmdPtr pCmd, CommitSession& session)
{
	VERIFY_MODEL(!pCmd->IsAction());

	Colour c = pCmd->GetColour();
	GetCmdStack(c).StartCmd(std::move(pCmd));

	GetCurrentCmd(c)->UpdateClient(session.GetController(), GetGame());
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

void UpkeepPhase::FinishTurn(CommitSession& session, const Player& player)
{
	LiveGame& game = GetGame();
	
	const Colour c = game.GetTeam(player).GetColour();

	VERIFY_MODEL(GetCurrentCmd(c) == nullptr);

	bool bOK = m_finished.insert(c).second;
	VERIFY_MODEL(bOK);

	const Controller& controller = session.GetController();

	UpdateClient(controller, &player);

	if (m_finished.size() == game.GetTeams().size())
	{
		session.DoAndPushRecord(RecordPtr(new StartRoundRecord));

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
