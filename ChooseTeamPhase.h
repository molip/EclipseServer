#pragma once

#include "Phase.h"

enum class RaceType;

class ChooseTeamPhase : public Phase, public TurnPhase
{
public:
	ChooseTeamPhase(LiveGame* pGame = nullptr);

	virtual void UpdateClient(const Controller& controller, const Player* pPlayer) const override;

	void AssignTeam(CommitSession& session, Player& player, RaceType race, Colour colour);
	const Team& GetCurrentTeam() const;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

protected:
	virtual CmdStack& GetCmdStack(Colour c) override { VERIFY(false); return *((CmdStack*)nullptr); }
};

