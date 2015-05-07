#pragma once

#include "Phase.h"

class UpkeepPhase : public Phase
{
public:
	UpkeepPhase(LiveGame* pGame = nullptr);

	virtual void Init(CommitSession& session) override;

	virtual void StartCmd(CmdPtr pCmd, CommitSession& session) override;

	virtual void UpdateClient(const Controller& controller, const Player* pPlayer) const override;
	virtual std::vector<const Team*> GetCurrentTeams() const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	void FinishUpkeep(CommitSession& session, const Player& player);
	void FinishGraveyard(CommitSession& session, const Player& player);

protected:
	virtual void OnCmdFinished(const Cmd& cmd, CommitSession& session) override;
	virtual CmdStack& GetCmdStack(Colour c) override;
	const CmdStack& GetCmdStack(Colour c) const { return const_cast<UpkeepPhase*>(this)->GetCmdStack(c); }

private:
	std::map<Colour, CmdStackPtr> m_cmdStacks;
	std::set<Colour> m_finished;
};

