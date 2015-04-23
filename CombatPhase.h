#pragma once

#include "OrderedPhase.h"
#include "Battle.h"

class Hex;

class CombatPhase : public OrderedPhase
{
public:
	CombatPhase(LiveGame* pGame = nullptr);

	virtual ~CombatPhase();

	virtual void Init(CommitSession& session) override;

	virtual void UpdateClient(const Controller& controller, const Player* pPlayer) const override;

	void FinishTurn(CommitSession& session);

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

protected:
	virtual const Team& GetCurrentTeam() const override;
	virtual void OnCmdFinished(const Cmd& cmd, CommitSession& session) override;

private:
	const Hex* GetNextBattleHex() const;
	void StartBattle(CommitSession& session, const Battle* oldBattle = nullptr);
	void StartTurn(CommitSession& session);
	void FinishBattle(CommitSession& session);

	int m_lastPopulationBattleHexId;
};
