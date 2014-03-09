#include "stdafx.h"
#include "UpgradeCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"

UpgradeCmd::UpgradeCmd(Colour colour, const LiveGame& game) : Cmd(colour)
{
}

void UpgradeCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseUpgrade(), GetPlayer(game));
}

CmdPtr UpgradeCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdUpgrade>(msg);
	
	// TODO: Reaction

	return nullptr;
}

void UpgradeCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
}

void UpgradeCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
}

REGISTER_DYNAMIC(UpgradeCmd)

//-----------------------------------------------------------------------------

class UpgradeRecord : public TeamRecord
{
public:
	UpgradeRecord() {}
	UpgradeRecord(Colour colour) : TeamRecord(colour) {}

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
	virtual std::string GetTeamMessage(bool bUndo) const
	{
		return FormatString("Upgrade");
	}
};

REGISTER_DYNAMIC(UpgradeRecord)
