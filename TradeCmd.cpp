#include "stdafx.h"
#include "TradeCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"
#include "CommitSession.h"

class TradeRecord : public TeamRecord
{
public:
	typedef std::pair<Resource, int> Pair;
	typedef std::vector<Pair> PairVec;
	TradeRecord() {}
	TradeRecord(Colour colour, const PairVec& srcs, const Pair& dst) : TeamRecord(colour), m_srcs(srcs), m_dst(dst) {}

	virtual void Save(Serial::SaveNode& node) const override
	{
		__super::Save(node);
		node.SavePairs("srcs", m_srcs, Serial::EnumSaver(), Serial::TypeSaver());
		node.SavePair("dst", m_dst, Serial::EnumSaver(), Serial::TypeSaver());
	}

	virtual void Load(const Serial::LoadNode& node) override
	{
		__super::Load(node);
		node.LoadPairs("srcs", m_srcs, Serial::EnumLoader(), Serial::TypeLoader());
		node.LoadPair("dst", m_dst, Serial::EnumLoader(), Serial::TypeLoader());
	}

private:
	virtual void Apply(bool bDo, Game& game, const RecordContext& context) override
	{
		Team& team = game.GetTeam(m_colour);

		for (auto& src : m_srcs)
			team.GetStorage()[src.first] -= bDo ? src.second : -src.second;

		team.GetStorage()[m_dst.first] += bDo ? m_dst.second : -m_dst.second;

		context.SendMessage(Output::UpdateStorageTrack(team));
	}

	virtual std::string GetTeamMessage() const
	{
		return FormatString("traded");
	}

	PairVec m_srcs;
	Pair m_dst;
};

REGISTER_DYNAMIC(TradeRecord)

//-----------------------------------------------------------------------------

TradeCmd::TradeCmd(Colour colour, const LiveGame& game) : Cmd(colour)
{
}

void TradeCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseTrade(GetTeam(game)), GetPlayer(game));
}

CmdPtr TradeCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	const Team& team = session.GetGame().GetTeam(m_colour);
	auto& storage = team.GetStorage();

	auto& m = VerifyCastInput<const Input::CmdTrade>(msg);

	const int fromCount = m.m_count * Race(team.GetRace()).GetTradeRate();
	VERIFY_INPUT_MSG("no count", m.m_count > 0);
	VERIFY_INPUT_MSG("to == from", m.m_from != m.m_to);
	VERIFY_INPUT_MSG("can't afford trade", fromCount <= storage[m.m_from]);

	TradeRecord::PairVec srcs{ { m.m_from, fromCount } };
	TradeRecord::Pair dst(m.m_to, m.m_count);

	TradeRecord* pRec = new TradeRecord(m_colour, srcs, dst);
	DoRecord(RecordPtr(pRec), session);

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
