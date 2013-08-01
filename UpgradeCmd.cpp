#include "stdafx.h"
#include "UpgradeCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"

UpgradeCmd::UpgradeCmd(Colour colour, LiveGame& game) : Cmd(colour)
{
}

void UpgradeCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseUpgrade(), GetPlayer(game));
}

CmdPtr UpgradeCmd::Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game)
{
	auto& m = CastThrow<const Input::CmdUpgrade>(msg);
	
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

class UpgradeRecord : public Record
{
public:
	UpgradeRecord() : m_colour(Colour::None) {}
	UpgradeRecord(Colour colour) : m_colour(colour) {}

	virtual void Save(Serial::SaveNode& node) const override 
	{
	}
	
	virtual void Load(const Serial::LoadNode& node) override 
	{
	}

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) override
	{
	}

	Colour m_colour;
};

REGISTER_DYNAMIC(UpgradeRecord)
