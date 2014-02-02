#pragma once

#include "Phase.h"

enum class RaceType;

class ChooseTeamPhase : public TurnPhase
{
public:
	ChooseTeamPhase();
	virtual void AddCmd(CmdPtr pCmd) override {}
	virtual void FinishCmd(Colour c) override {}
	virtual Cmd* RemoveCmd(Colour c) override { return nullptr; }; // Returns cmd to undo.
	virtual bool CanRemoveCmd(Colour c) const override { return false; }
	virtual bool IsTeamActive(Colour c) const override;

	virtual Cmd* GetCurrentCmd(Colour c) override { return nullptr; }

	void AssignTeam(Player& player, RaceType race, Colour colour);

	virtual void Save(Serial::SaveNode& node) const override {}
	virtual void Load(const Serial::LoadNode& node) override {}
};

