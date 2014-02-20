#include "stdafx.h"
#include "TradeCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"

class TradeRecord : public Record
{
public:
	typedef std::pair<Resource, int> Pair;
	typedef std::vector<Pair> PairVec;
	TradeRecord() {}
	TradeRecord(Colour colour, const PairVec& srcs, const Pair& dst) : Record(colour), m_srcs(srcs), m_dst(dst) {}

	virtual void Save(Serial::SaveNode& node) const override
	{
		__super::Save(node);
		node.SavePairs("srcs", m_srcs, Serial::EnumSaver(), Serial::TypeSaver());
		node.SavePair("dst", m_dst, Serial::EnumSaver(), Serial::TypeSaver());
	}

	virtual void Load(const Serial::LoadNode& node) override
	{
		__super::Load(node);
		node.LoadPairs("srcs", m_srcs, Serial::EnumSaver(), Serial::TypeSaver());
		node.LoadPair("dst", m_dst, Serial::EnumSaver(), Serial::TypeSaver());
	}

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) override
	{
		Team& team = game.GetTeam(m_colour);

		for (auto& src : m_srcs)
			team.GetStorage()[src.first] -= bDo ? src.second : -src.second;

		team.GetStorage()[m_dst.first] += bDo ? m_dst.second : -m_dst.second;

		controller.SendMessage(Output::UpdateStorageTrack(team), game);
	}

	PairVec m_srcs;
	const Pair m_dst;
};

REGISTER_DYNAMIC(TradeRecord)

//-----------------------------------------------------------------------------

TradeCmd::TradeCmd(Colour colour, LiveGame& game) : Cmd(colour)
{
}

void TradeCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseTrade(GetTeam(game)), GetPlayer(game));
}

CmdPtr TradeCmd::Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game)
{
	const Team& team = game.GetTeam(m_colour);
	auto& storage = team.GetStorage();

	auto& m = CastThrow<const Input::CmdTrade>(msg);

	const int fromCount = m.m_count * Race(team.GetRace()).GetTradeRate();
	AssertThrow("TradeCmd::Process: no count", m.m_count > 0);
	AssertThrow("TradeCmd::Process: to == from", m.m_from != m.m_to);
	AssertThrow("TradeCmd::Process: can't afford trade", fromCount <= storage[m.m_from]);

	TradeRecord::PairVec srcs{ { m.m_from, fromCount } };
	TradeRecord::Pair dst(m.m_to, m.m_count);

	TradeRecord* pRec = new TradeRecord(m_colour, srcs, dst);
	DoRecord(RecordPtr(pRec), controller, game);

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
