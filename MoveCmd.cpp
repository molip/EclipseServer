#include "stdafx.h"
#include "MoveCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"
#include "CommitSession.h"

MoveCmd::MoveCmd(Colour colour, const LiveGame& game, int iPhase) : PhaseCmd(colour, iPhase)
{
}

bool MoveCmd::CanMoveFrom(const Hex& hex, const LiveGame& game) const
{
	return hex.HasShip(m_colour, true) && hex.CanMoveOut(m_colour) && 
		hex.HasNeighbour(game.GetMap(), GetTeam(game).HasTech(TechType::WormholeGen));
}

void MoveCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	std::map<MapPos, std::set<ShipType>> srcs;

	// Get movable ships in each hex.
	for (auto& h : game.GetMap().GetHexes()) // Pos, hex.
		if (CanMoveFrom(*h.second, game))
			for (auto& s : h.second->GetShips())
				if (s.GetColour() == m_colour)
					srcs[h.first].insert(s.GetType());

	controller.SendMessage(Output::ChooseMoveSrc(srcs, m_iPhase > 0), GetPlayer(game));
}

CmdPtr MoveCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	if (dynamic_cast<const Input::CmdAbort*>(&msg))
		return nullptr;

	auto& m = VerifyCastInput<const Input::CmdMoveSrc>(msg);
	MapPos pos(m.m_x, m.m_y);
	
	const LiveGame& game = session.GetGame(); 
	const Hex& hex = game.GetMap().GetHex(pos);
	VerifyInput("MoveCmd::Process: invalid hex", CanMoveFrom(hex, game));
	VerifyInput("MoveCmd::Process: invalid ship", hex.HasShip(m_colour, m.m_ship));

	return CmdPtr(new MoveDstCmd(m_colour, game, pos, m.m_ship, m_iPhase));
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

class MoveRecord : public TeamRecord
{
public:
	MoveRecord() {}
	MoveRecord(Colour colour, ShipType ship, const MapPos& src, const MapPos& dst) : 
		TeamRecord(colour), m_ship(ship), m_src(src), m_dst(dst) {}

	virtual void Save(Serial::SaveNode& node) const override 
	{
		__super::Save(node);
		node.SaveEnum("ship", m_ship);
		node.SaveType("src_pos", m_src);
		node.SaveType("dst_pos", m_dst);
	}
	
	virtual void Load(const Serial::LoadNode& node) override 
	{
		__super::Load(node);
		node.LoadEnum("ship", m_ship);
		node.LoadType("src_pos", m_src);
		node.LoadType("dst_pos", m_dst);
	}

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) override
	{
		Hex& srcHex = game.GetMap().GetHex(bDo ? m_src : m_dst);
		Hex& dstHex = game.GetMap().GetHex(bDo ? m_dst : m_src);

		srcHex.RemoveShip(m_ship, m_colour);
		dstHex.AddShip(m_ship, m_colour);

		controller.SendMessage(Output::UpdateMap(game), game);
	}

	ShipType m_ship;
	MapPos m_src, m_dst;
};

REGISTER_DYNAMIC(MoveRecord)

//-----------------------------------------------------------------------------

MoveDstCmd::MoveDstCmd(Colour colour, const LiveGame& game, const MapPos& src, ShipType ship, int iPhase) : 
	PhaseCmd(colour, iPhase), m_src(src), m_ship(ship)
{
}

void MoveDstCmd::AddNeighbourDsts(const LiveGame& game, const MapPos& pos, int movement, std::set<MapPos>& dsts) const
{
	if (movement <= 0)
		return;

	const Map& map = game.GetMap();

	if (map.GetHex(pos).CanMoveThrough(m_colour))
		for (auto& n : map.GetNeighbours(pos, GetTeam(game).HasTech(TechType::WormholeGen)))
			if (n != m_src)
				if (dsts.insert(n).second)
					AddNeighbourDsts(game, n, movement - 1, dsts);
}

std::set<MapPos> MoveDstCmd::GetDsts(const LiveGame& game) const
{
	std::set<MapPos> dsts;
	AddNeighbourDsts(game, m_src, GetTeam(game).GetBlueprint(m_ship).GetMovement(), dsts);
	return dsts;
}

void MoveDstCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	std::set<MapPos> dsts = GetDsts(game);
	controller.SendMessage(Output::ChooseMoveDst(dsts), GetPlayer(game));
}

CmdPtr MoveDstCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdMoveDst>(msg);
	MapPos dst(m.m_x, m.m_y);

	const LiveGame& game = session.GetGame();
	std::set<MapPos> dsts = GetDsts(game);
	VerifyInput("MoveDstCmd::Process: invalid hex", dsts.find(dst) != dsts.end());

	MoveRecord* pRec = new MoveRecord(m_colour, m_ship, m_src, dst);
	DoRecord(RecordPtr(pRec), session);

	const Team& team = GetTeam(game);
	if (!team.HasPassed() && m_iPhase + 1 < Race(team.GetRace()).GetMoves())
		return CmdPtr(new MoveCmd(m_colour, game, m_iPhase + 1));

	return nullptr;
}

void MoveDstCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveType("src_pos", m_src);
	node.SaveEnum("ship", m_ship);
}

void MoveDstCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadType("src_pos", m_src);
	node.LoadEnum("ship", m_ship);
}

REGISTER_DYNAMIC(MoveDstCmd)
