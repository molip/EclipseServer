#pragma once

#include "OrderedPhase.h"

class Hex;
class Battle;

class CombatPhase : public OrderedPhase
{
public:
	CombatPhase();
	CombatPhase(LiveGame* pGame);

	virtual ~CombatPhase();

	virtual void Init(CommitSession& session) override;

	void FinishTurn(CommitSession& session);

	const Battle& GetBattle() const { return *m_battle; }

	virtual void UpdateClient(const Controller& controller, const Player* pPlayer) const override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

protected:
	virtual const Team& GetCurrentTeam() const override;

private:
	bool StartBattle();

	std::unique_ptr<Battle> m_battle;
};
