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

	const Battle& battle = session.GetGame().GetCombatPhase().GetBattle();
	//const Battle::Group& turn = battle.GetCurrentGroup();

	VerifyInput("CombatCmd::Process: missiles must be fired", !battle.IsMissilePhase() || m.m_fire);
	


	Dice dice;
	battle.RollDice(session.GetGame(), dice);

	return nullptr;
}

void CombatCmd::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
}

void CombatCmd::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
}

REGISTER_DYNAMIC(CombatCmd)

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
