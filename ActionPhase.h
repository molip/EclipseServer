#pragma once

#include "Phase.h"

class ActionPhase : public TurnPhase
{
public:
	ActionPhase(LiveGame* pGame = nullptr);

	virtual ~ActionPhase();

	virtual void StartCmd(CmdPtr pCmd, Controller& controller) override;

	virtual void AddCmd(CmdPtr pCmd) override;
	virtual void FinishCmd(Colour c) override;
	virtual Cmd* RemoveCmd(const Controller& controller, Colour c) override; // Returns cmd to undo.
	virtual bool CanRemoveCmd(Colour c) const override;
	virtual bool IsTeamActive(Colour c) const override;

	virtual Cmd* GetCurrentCmd(Colour c) override;

	Cmd* GetCurrentCmd();
	const Cmd* GetCurrentCmd() const { return const_cast<ActionPhase*>(this)->GetCurrentCmd(); }
	bool CanRemoveCmd() const;

	bool HasDoneAction() const { return m_bDoneAction; }
	bool CanDoAction() const;
	Cmd* GetCurrentPlayerCmd();
	const Cmd* GetCurrentPlayerCmd() const { return const_cast<ActionPhase*>(this)->GetCurrentPlayerCmd(); }
	void FinishTurn(Controller& controller);

	virtual void UpdateClient(const Controller& controller, const Player* pPlayer) const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	std::vector<Colour> m_passOrder;

	CmdStack* m_pCmdStack;
	bool m_bDoneAction;
};

DEFINE_UNIQUE_PTR(Phase)