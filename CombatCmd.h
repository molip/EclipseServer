#pragma once

#include "Cmd.h"
#include "Dice.h"

class CombatCmd : public Cmd
{
public:
	CombatCmd() {}
	CombatCmd(Colour colour, const LiveGame& game);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;
	virtual bool CanUndo() const override { return false; }
};

class CombatDiceCmd : public Cmd
{
public:
	CombatDiceCmd() {}
	CombatDiceCmd(Colour colour, const LiveGame& game, const Dice& dice);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;
	virtual bool CanUndo() const override { return false; }

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	Dice m_dice;
};
