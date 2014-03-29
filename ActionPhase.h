#pragma once

#include "OrderedPhase.h"
#include "HexArrivals.h"

class Hex;

class ActionPhase : public OrderedPhase, public TurnPhase
{
public:
	ActionPhase(LiveGame* pGame = nullptr);

	virtual ~ActionPhase();

	virtual void StartCmd(CmdPtr pCmd, CommitSession& session) override;
	virtual const Team& GetCurrentTeam() const override;

	bool HasDoneAction() const { return m_bDoneAction; }
	bool CanDoAction() const;
	void FinishTurn(CommitSession& session);

	void ShipMovedFrom(const Hex& hex, Colour colour);
	void ShipMovedTo(const Hex& hex, Colour colour);

	virtual void UpdateClient(const Controller& controller, const Player* pPlayer) const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

protected:
	virtual Cmd* RemoveCmd(CommitSession& session, Colour c) override; // Returns cmd to undo.

private:
	std::vector<Colour> m_passOrder;
	HexArrivals m_hexArrivalOrder;
	bool m_bDoneAction;
};
