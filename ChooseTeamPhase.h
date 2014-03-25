#pragma once

#include "Phase.h"

enum class RaceType;

class ChooseTeamPhase : public TurnPhase
{
public:
	ChooseTeamPhase(LiveGame* pGame = nullptr);

	virtual bool CanRemoveCmd(Colour c) const override { return false; }

	virtual Cmd* GetCurrentCmd(Colour c) override { return nullptr; }

	virtual void UpdateClient(const Controller& controller, const Player* pPlayer) const override;

	void AssignTeam(CommitSession& session, Player& player, RaceType race, Colour colour);

	virtual void Save(Serial::SaveNode& node) const override {}
	virtual void Load(const Serial::LoadNode& node) override {}

protected:
	virtual void AddCmd(CmdPtr pCmd) override {}
	virtual void FinishCmd(Colour c) override {}
	virtual Cmd* RemoveCmd(CommitSession& session, Colour c) override { return nullptr; }; // Returns cmd to undo.
};

