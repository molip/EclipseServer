#include "stdafx.h"
#include "CombatCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"
#include "CommitSession.h"
#include "CombatPhase.h"
#include "Battle.h"
#include "Dice.h"
#include "Player.h"
#include "AttackRecord.h"

CombatCmd::CombatCmd(Colour colour, const LiveGame& game) : Cmd(colour)
{
}

void CombatCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseCombat(game), GetPlayer(game));
}

CmdPtr CombatCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdCombat>(msg);

	const Battle& battle = session.GetGame().GetBattle();

	VERIFY_INPUT_MSG("missiles must be fired", !battle.IsMissilePhase() || m.m_fire);
	
	Dice dice;
	battle.RollDice(session.GetGame(), dice);

	return CmdPtr(new CombatDiceCmd(m_colour, session.GetGame(), dice));
}

REGISTER_DYNAMIC(CombatCmd)

//-----------------------------------------------------------------------------

CombatDiceCmd::CombatDiceCmd(Colour colour, const LiveGame& game, const Dice& dice) : Cmd(colour), m_dice(dice)
{
}

void CombatDiceCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	// Send to all players. 
	controller.SendMessage(Output::ChooseDice(game, m_dice, GetPlayer(game).GetID()), game);
}

CmdPtr CombatDiceCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdDice>(msg);

	session.GetController().SendMessage(Output::ChooseFinished(), GetPlayer(session.GetGame()));

	const Battle::Hits hits = session.GetGame().GetBattle().AutoAssignHits(m_dice, session.GetGame());

	// TODO: Send attack animation 

	DoRecord(RecordPtr(new AttackRecord(hits)), session);

	return nullptr;
}

void CombatDiceCmd::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveClass("dice", m_dice);
}

void CombatDiceCmd::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadClass("dice", m_dice);
}

REGISTER_DYNAMIC(CombatDiceCmd)

//-----------------------------------------------------------------------------

class CombatRecord : public TeamRecord
{
public:
	CombatRecord() {}
	CombatRecord(Colour colour) : TeamRecord(colour) {}

	virtual void Save(Serial::SaveNode& node) const override 
	{
		__super::Save(node);
	}
	
	virtual void Load(const Serial::LoadNode& node) override 
	{
		__super::Load(node);
	}

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) override
	{
	}

	virtual std::string GetTeamMessage() const
	{
		return FormatString("did some combat");
	}
};

REGISTER_DYNAMIC(CombatRecord)
