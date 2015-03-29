#include "stdafx.h"
#include "AutoInfluenceCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "Player.h"
#include "Race.h"
#include "LiveGame.h"
#include "Map.h"
#include "CommitSession.h"
#include "InfluenceRecord.h"

AutoInfluenceCmd::AutoInfluenceCmd(Colour colour, const LiveGame& game) : Cmd(colour)
{
	VERIFY_MODEL(GetTeam(game).HasPassed());
}

Colour AutoInfluenceCmd::GetAutoInfluenceColour(const Hex& hex)
{
	if (!hex.IsOwned() && hex.GetFleets().size() == 1)
		return hex.GetFleets().front().GetColour();
	return Colour::None;
}

std::vector<MapPos> AutoInfluenceCmd::GetHexes(const LiveGame& game) const
{
	auto& team = GetTeam(game);

	std::vector<MapPos> hexes;

	for (auto& kv : game.GetMap().GetHexes())
	{
		auto& hex = kv.second;
		if (GetAutoInfluenceColour(*hex) == m_colour)
			hexes.push_back(hex->GetPos());
	}

	return hexes;
}

void AutoInfluenceCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	bool canChooseTrack = GetTeam(game).GetInfluenceTrack().GetDiscCount() > 0;
	controller.SendMessage(Output::ChooseAutoInfluence(GetHexes(game)), GetPlayer(game));
}

Cmd::ProcessResult AutoInfluenceCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	if (dynamic_cast<const Input::CmdAbort*>(&msg))
		return nullptr;

	std::vector<MapPos> hexes = GetHexes(session.GetGame());

	auto& m = VerifyCastInput<const Input::CmdAutoInfluence>(msg);
	VERIFY_INPUT(m.m_selected.size() == hexes.size());

	const LiveGame& game = session.GetGame();

	for (size_t i = 0; i < hexes.size();++i)
		if (m.m_selected[i])
			DoRecord(RecordPtr(new InfluenceRecord(m_colour, nullptr, &hexes[i])), session);

	return nullptr;
}

REGISTER_DYNAMIC(AutoInfluenceCmd)
