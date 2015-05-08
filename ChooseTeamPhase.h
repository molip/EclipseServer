#pragma once

#include "Phase.h"

enum class RaceType;

class ChooseTeamPhase : public Phase, public TurnPhase
{
public:
	ChooseTeamPhase(LiveGame* pGame = nullptr);

	virtual void UpdateClient(const Controller& controller, const Player* pPlayer) const override;
	virtual void Init(CommitSession& session) override;
	virtual std::vector<const Team*> GetCurrentTeams() const { return{ &GetCurrentTeam() }; }

	void AssignTeam(CommitSession& session, Player& player, RaceType race, Colour colour);
	const Team& GetCurrentTeam() const;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

protected:
	virtual CmdStack& GetCmdStack(Colour c) override { VERIFY(false); return *((CmdStack*)nullptr); }
	const Player& ChooseTeamPhase::GetCurrentPlayer() const;
};

