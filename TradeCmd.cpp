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
	TradeRecord() {}
	TradeRecord(Colour colour) : Record(colour) {}

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

REGISTER_DYNAMIC(TradeRecord)
