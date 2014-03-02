#pragma once

#include "Phase.h"

class UpkeepPhase : public Phase
{
public:
	UpkeepPhase(LiveGame* pGame = nullptr);

	virtual void StartCmd(CmdPtr pCmd, CommitSession& session) override;

	virtual void AddCmd(CmdPtr pCmd) override;
	virtual void FinishCmd(Colour c) override;
	virtual Cmd* RemoveCmd(CommitSession& session, Colour c) override; // Returns cmd to undo.
	virtual bool CanRemoveCmd(Colour c) const override;
	virtual bool IsTeamActive(Colour c) const override;

	virtual Cmd* GetCurrentCmd(Colour c) override;

	virtual void UpdateClient(const Controller& controller, const Player* pPlayer) const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	void FinishTurn(CommitSession& session, const Player& player);

private:
	CmdStack& GetCmdStack(Colour c);
	const CmdStack& GetCmdStack(Colour c) const { return const_cast<UpkeepPhase*>(this)->GetCmdStack(c); }

	std::map<Colour, CmdStackPtr> m_cmdStacks;
	std::set<Colour> m_finished;
};

