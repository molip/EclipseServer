#include "stdafx.h"
#include "BankruptCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "Player.h"
#include "Race.h"
#include "LiveGame.h"
#include "Map.h"
#include "DiscoverCmd.h"
#include "CommitSession.h"
#include "UncoloniseCmd.h"
#include "InfluenceRecord.h"

BankruptCmd::BankruptCmd(Colour colour, const LiveGame& game) : Cmd(colour)
{
	VERIFY_MODEL(GetTeam(game).HasPassed());
	VERIFY_MODEL(GetTeam(game).IsBankrupt());
}

std::vector<MapPos> BankruptCmd::GetSources(const LiveGame& game) const
{
	return game.GetMap().GetOwnedHexPositions(GetTeam(game));
}

void BankruptCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	bool canChooseTrack = GetTeam(game).GetInfluenceTrack().GetDiscCount() > 0;
	controller.SendMessage(Output::ChooseInfluenceSrc(GetSources(game), false, false, 0), GetPlayer(game));
}

Cmd::ProcessResult BankruptCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdInfluenceSrc>(msg);

	std::vector<MapPos> srcs = BankruptCmd::GetSources(session.GetGame());
	VERIFY_INPUT_MSG("invalid pos index", m.m_iPos == -1 || InRange(srcs, m.m_iPos));

	auto& pos = srcs[m.m_iPos];

	InfluenceRecord* pRec = new InfluenceRecord(m_colour, &pos, nullptr);
	DoRecord(RecordPtr(pRec), session);

	const LiveGame& game = session.GetGame();
	const Map& map = game.GetMap();
	if (map.GetHex(pos).HasPopulation())
		return ProcessResult(new UncoloniseCmd(m_colour, pos), nullptr);

	return nullptr;
}

REGISTER_DYNAMIC(BankruptCmd)
