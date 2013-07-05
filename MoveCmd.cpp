#include "stdafx.h"
#include "MoveCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"

MoveCmd::MoveCmd(Colour colour, LiveGame& game) : Cmd(colour)
{
}

void MoveCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseMove(), GetPlayer(game));
}

CmdPtr MoveCmd::Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game)
{
	auto& m = CastThrow<const Input::CmdMove>(msg);
	
	return nullptr;
}

void MoveCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
}

void MoveCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
}

REGISTER_DYNAMIC(MoveCmd)

//-----------------------------------------------------------------------------

class MoveRecord : public Record
{
public:
	MoveRecord() : m_colour(Colour::None) {}
	MoveRecord(Colour colour) : m_colour(colour) {}

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

REGISTER_DYNAMIC(MoveRecord)
