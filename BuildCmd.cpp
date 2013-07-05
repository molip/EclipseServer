#include "stdafx.h"
#include "BuildCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"

BuildCmd::BuildCmd(Colour colour, LiveGame& game) : Cmd(colour)
{
}

void BuildCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseBuild(), GetPlayer(game));
}

CmdPtr BuildCmd::Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game)
{
	auto& m = CastThrow<const Input::CmdBuild>(msg);
	
	return nullptr;
}

void BuildCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
}

void BuildCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
}

REGISTER_DYNAMIC(BuildCmd)

//-----------------------------------------------------------------------------

class BuildRecord : public Record
{
public:
	BuildRecord() : m_colour(Colour::None) {}
	BuildRecord(Colour colour) : m_colour(colour) {}

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

REGISTER_DYNAMIC(BuildRecord)
