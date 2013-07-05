#include "stdafx.h"
#include "ResearchCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"

ResearchCmd::ResearchCmd(Colour colour, LiveGame& game) : Cmd(colour)
{
}

void ResearchCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseResearch(), GetPlayer(game));
}

CmdPtr ResearchCmd::Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game)
{
	auto& m = CastThrow<const Input::CmdResearch>(msg);
	
	return nullptr;
}

void ResearchCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
}

void ResearchCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
}

REGISTER_DYNAMIC(ResearchCmd)

//-----------------------------------------------------------------------------

class ResearchRecord : public Record
{
public:
	ResearchRecord() : m_colour(Colour::None) {}
	ResearchRecord(Colour colour) : m_colour(colour) {}

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

REGISTER_DYNAMIC(ResearchRecord)
