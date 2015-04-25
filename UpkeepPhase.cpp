#include "UpkeepPhase.h"
#include "LiveGame.h"
#include "CmdStack.h"
#include "Cmd.h"
#include "Controller.h"
#include "Output.h"
#include "Player.h"
#include "StartRoundRecord.h"
#include "CommitSession.h"
#include "GraveyardCmd.h"
#include "IncomeRecord.h"
#include "InfluenceRecord.h"
#include "AutoInfluenceCmd.h"

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
	__super::StartCmd(std::move(pCmd), session);
}

void UpkeepPhase::Init(CommitSession& session)
{
	auto& game = session.GetGame();
	std::set<Colour> influenceableHexes; // Team -> hex IDs.

	for (auto& pair : game.GetMap().GetHexes())
	{
		auto& hex = pair.second;

		// "If you have at least one Ship in a hex that has no population, remove the previous controller’s Influence Disc."
		if (hex->IsOwned() && !hex->HasPopulation() && hex->HasForeignShip(hex->GetColour()))
			session.DoAndPushRecord(RecordPtr(new InfluenceRecord(hex->GetColour(), &hex->GetPos(), nullptr)));

		// If at the end of the Combat Phase your Ship is in a hex without an Influence Disc, you may place a disc there."
		Colour c = AutoInfluenceCmd::GetAutoInfluenceColour(*hex);
		if (c != Colour::None)
			influenceableHexes.insert(c);
	}

	for (auto& c : influenceableHexes)
		StartCmd(CmdPtr(new AutoInfluenceCmd(c, game)), session); // TODO: Stop user undoing.
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

void UpkeepPhase::FinishUpkeep(CommitSession& session, const Player& player)
{
	const Team& team = GetGame().GetTeam(player);
	session.DoAndPushRecord(RecordPtr(new IncomeRecord(team.GetColour())));

	if (team.GetGraveyard().IsEmpty())
		FinishGraveyard(session, player);
	else
		StartCmd(CmdPtr(new GraveyardCmd(team.GetColour(), GetGame())), session);
}

void UpkeepPhase::OnCmdFinished(const Cmd& cmd, CommitSession& session)
{
	if (dynamic_cast<const GraveyardCmd*>(&cmd))
		FinishGraveyard(session, session.GetGame().GetTeam(cmd.GetColour()).GetPlayer());
}

void UpkeepPhase::FinishGraveyard(CommitSession& session, const Player& player)
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

		if (game.HasFinished())
			game.StartScorePhase(); // Deletes this.
		else
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
