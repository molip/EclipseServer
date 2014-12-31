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
	virtual void Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState) override
	{
		teamState.SetPassed(bDo);
	}

	virtual void Update(const Game& game, const Team& team, const RecordContext& context) const override
	{
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
