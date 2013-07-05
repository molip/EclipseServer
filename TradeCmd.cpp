#include "stdafx.h"
#include "TradeCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"

TradeCmd::TradeCmd(Colour colour, LiveGame& game) : Cmd(colour)
{
}

void TradeCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseTrade(), GetPlayer(game));
}

CmdPtr TradeCmd::Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game)
{
	auto& m = CastThrow<const Input::CmdTrade>(msg);
	
	return nullptr;
}

void TradeCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
}

void TradeCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
}

REGISTER_DYNAMIC(TradeCmd)

//-----------------------------------------------------------------------------

class TradeRecord : public Record
{
public:
	TradeRecord() : m_colour(Colour::None) {}
	TradeRecord(Colour colour) : m_colour(colour) {}

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

REGISTER_DYNAMIC(TradeRecord)
