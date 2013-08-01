#include "stdafx.h"
#include "PassCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"

class PassRecord : public Record
{
public:
	PassRecord() : m_colour(Colour::None) {}
	PassRecord(Colour colour) : m_colour(colour) {}

	virtual void Save(Serial::SaveNode& node) const override 
	{
		node.SaveEnum("colour", m_colour);
	}
	
	virtual void Load(const Serial::LoadNode& node) override 
	{
		node.LoadEnum("colour", m_colour);
	}

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) override
	{
		Team& team = game.GetTeam(m_colour);
		team.SetPassed(bDo);
		controller.SendMessage(Output::UpdatePassed(team), game);
	}

	Colour m_colour;
};

REGISTER_DYNAMIC(PassRecord)

//-----------------------------------------------------------------------------

PassCmd::PassCmd(Colour colour, LiveGame& game) : Cmd(colour)
{
}

CmdPtr PassCmd::Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game)
{	
	AssertThrow("PassCmd::Process", !GetTeam(game).HasPassed());

	Record* pRec = new PassRecord(m_colour);
	pRec->Do(game, controller);
	game.PushRecord(RecordPtr(pRec));

	return nullptr;
}

void PassCmd::Undo(const Controller& controller, LiveGame& game)
{
	AssertThrow("PassCmd::Undo", GetTeam(game).HasPassed());

	RecordPtr pRec = game.PopRecord();
	pRec->Undo(game, controller);
}

REGISTER_DYNAMIC(PassCmd)
