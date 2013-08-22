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
	PassRecord() {}
	PassRecord(Colour colour) : Record(colour) {}

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) override
	{
		Team& team = game.GetTeam(m_colour);
		team.SetPassed(bDo);
		controller.SendMessage(Output::UpdatePassed(team), game);
	}
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

REGISTER_DYNAMIC(PassCmd)
