#include "stdafx.h"
#include "PassCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"

class PassRecord : public TeamRecord
{
public:
	PassRecord() {}
	PassRecord(Colour colour) : TeamRecord(colour) {}

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

PassCmd::PassCmd(Colour colour, const LiveGame& game) : Cmd(colour)
{
}

CmdPtr PassCmd::Process(const Input::CmdMessage& msg, const Controller& controller, const LiveGame& game)
{	
	VerifyInput("PassCmd::Process", !GetTeam(game).HasPassed());

	Record* pRec = new PassRecord(m_colour);
	DoRecord(RecordPtr(pRec), controller, game);

	return nullptr;
}

REGISTER_DYNAMIC(PassCmd)
