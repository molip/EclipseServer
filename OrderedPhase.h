#pragma once

#include "Phase.h"

class OrderedPhase : public Phase
{
public:
	OrderedPhase(LiveGame* pGame);

	virtual ~OrderedPhase();

	virtual void StartCmd(CmdPtr pCmd, CommitSession& session) override;

	bool IsTeamActive(Colour c) const;

	Cmd* GetCurrentCmd();
	const Cmd* GetCurrentCmd() const { return const_cast<OrderedPhase*>(this)->GetCurrentCmd(); }
	bool CanRemoveCmd() const;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

protected:
	virtual CmdStack& GetCmdStack(Colour c) override;

	virtual const Team& GetCurrentTeam() const = 0;

	const Player& GetCurrentPlayer() const;

	CmdStack* m_pCmdStack;
};
