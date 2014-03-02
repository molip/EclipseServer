#include "stdafx.h"
#include "DiplomacyCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"

DiplomacyCmd::DiplomacyCmd(Colour colour, const LiveGame& game) : Cmd(colour)
{
}

void DiplomacyCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseDiplomacy(), GetPlayer(game));
}

CmdPtr DiplomacyCmd::Process(const Input::CmdMessage& msg, const Controller& controller, const LiveGame& game)
{
	auto& m = VerifyCastInput<const Input::CmdDiplomacy>(msg);
	
	return nullptr;
}

void DiplomacyCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
}

void DiplomacyCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
}

REGISTER_DYNAMIC(DiplomacyCmd)

//-----------------------------------------------------------------------------

class DiplomacyRecord : public TeamRecord
{
public:
	DiplomacyRecord() {}
	DiplomacyRecord(Colour colour) : TeamRecord(colour) {}

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
};

REGISTER_DYNAMIC(DiplomacyRecord)
