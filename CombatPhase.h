#pragma once

#include "OrderedPhase.h"
#include "Battle.h"

class Hex;

class CombatPhase : public OrderedPhase
{
public:
	CombatPhase();
	CombatPhase(LiveGame* pGame);

	virtual ~CombatPhase();

	virtual void Init(CommitSession& session) override;

	virtual void UpdateClient(const Controller& controller, const Player* pPlayer) const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

protected:
	virtual const Team& GetCurrentTeam() const override;
	virtual void OnCmdFinished(CommitSession& session) override;

private:
	void StartBattle(CommitSession& session, const Battle* oldBattle = nullptr);
	void StartTurn(CommitSession& session);
	void FinishTurn(CommitSession& session);
	void FinishBattle(CommitSession& session);
};
