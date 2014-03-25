#pragma once

#include "Phase.h"

class Hex;

class ActionPhase : public TurnPhase
{
public:
	ActionPhase(LiveGame* pGame = nullptr);

	virtual ~ActionPhase();

	virtual void StartCmd(CmdPtr pCmd, CommitSession& session) override;
	virtual bool CanRemoveCmd(Colour c) const override;
	virtual Cmd* GetCurrentCmd(Colour c) override;

	bool IsTeamActive(Colour c) const;

	Cmd* GetCurrentCmd();
	const Cmd* GetCurrentCmd() const { return const_cast<ActionPhase*>(this)->GetCurrentCmd(); }
	bool CanRemoveCmd() const;

	bool HasDoneAction() const { return m_bDoneAction; }
	bool CanDoAction() const;
	void FinishTurn(CommitSession& session);

	void ShipMovedFrom(const Hex& hex, Colour colour);
	void ShipMovedTo(const Hex& hex, Colour colour);

	virtual void UpdateClient(const Controller& controller, const Player* pPlayer) const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

protected:
	virtual void AddCmd(CmdPtr pCmd) override;
	virtual void FinishCmd(Colour c) override;
	virtual Cmd* RemoveCmd(CommitSession& session, Colour c) override; // Returns cmd to undo.

private:
	std::vector<Colour> m_passOrder;
	std::map<int, std::vector<Colour>> m_hexArrivalOrder;

	CmdStack* m_pCmdStack;
	bool m_bDoneAction;
};

DEFINE_UNIQUE_PTR(Phase)