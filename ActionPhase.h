#pragma once

#include "OrderedPhase.h"

class Hex;

class ActionPhase : public OrderedPhase, public TurnPhase
{
public:
	ActionPhase(LiveGame* pGame = nullptr);

	virtual ~ActionPhase();

	virtual void StartCmd(CmdPtr pCmd, CommitSession& session) override;
	virtual const Team& GetCurrentTeam() const override;
	virtual void Init(CommitSession& session) override;

	bool HasDoneAction() const { return m_bDoneAction; }
	bool CanDoAction() const;
	void FinishTurn(CommitSession& session);

	virtual void UpdateClient(const Controller& controller, const Player* pPlayer) const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

protected:
	virtual Cmd* RemoveCmd(CommitSession& session, Colour c) override; // Returns cmd to undo.

private:
	std::vector<Colour> m_passOrder;
	bool m_bDoneAction;
};
