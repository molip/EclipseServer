#include "stdafx.h"
#include "PassCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"
#include "CommitSession.h"

class PassRecord : public TeamRecord
{
public:
	PassRecord() {}
	PassRecord(Colour colour) : TeamRecord(colour) {}

private:
	virtual void Apply(bool bDo, Game& game, const RecordContext& context) override
	{
		Team& team = game.GetTeam(m_colour);
		team.SetPassed(bDo);
		context.SendMessage(Output::UpdatePassed(team));
	}

	virtual std::string GetTeamMessage() const
	{
		return FormatString("passed");
	}
};

REGISTER_DYNAMIC(PassRecord)

//-----------------------------------------------------------------------------

PassCmd::PassCmd(Colour colour, const LiveGame& game) : Cmd(colour)
{
}

CmdPtr PassCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{	
	VERIFY_INPUT(!GetTeam(session.GetGame()).HasPassed());

	Record* pRec = new PassRecord(m_colour);
	DoRecord(RecordPtr(pRec), session);

	return nullptr;
}

REGISTER_DYNAMIC(PassCmd)
