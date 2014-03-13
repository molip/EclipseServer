#include "stdafx.h"
#include "ExploreCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "Player.h"
#include "Race.h"
#include "LiveGame.h"
#include "Map.h"
#include "DiscoverCmd.h"
#include "Record.h"
#include "EdgeSet.h"
#include "ActionPhase.h"
#include "CommitSession.h"

class ExploreRecord : public TeamRecord
{
public:
	ExploreRecord() : m_idHex(-1), m_hexRing(HexRing::None) {}
	ExploreRecord(Colour colour, HexRing hexRing) : TeamRecord(colour), m_idHex(-1), m_hexRing(hexRing) {}

	int GetHexID() const { return m_idHex; }
		
private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) override
	{
		if (bDo)
			m_idHex = game.GetHexBag(m_hexRing).TakeTile();
		else
		{
			game.GetHexBag(m_hexRing).ReturnTile(m_idHex);
			m_idHex = -1;
		}
	}

	virtual void Save(Serial::SaveNode& node) const override
	{
		__super::Save(node);
		node.SaveType("hex_id", m_idHex);
		node.SaveEnum("hex_ring", m_hexRing);
	}

	virtual void Load(const Serial::LoadNode& node) override
	{
		__super::Load(node);
		node.LoadType("hex_id", m_idHex);
		node.LoadEnum("hex_ring", m_hexRing);
	}

	virtual std::string GetTeamMessage() const
	{
		return FormatString("received hex %0", m_idHex);
	}

	int m_idHex;
	HexRing m_hexRing;
};
REGISTER_DYNAMIC(ExploreRecord)

ExploreCmd::ExploreCmd(Colour colour, const LiveGame& game, int iPhase) : PhaseCmd(colour, iPhase), m_idHex(-1), m_iPos(-1)
{
	const Team& team = GetTeam(game);
	VerifyInput("ExploreCmd::ExploreCmd", !team.HasPassed());

	std::set<MapPos> positions;
	const Map::HexMap& hexes = game.GetMap().GetHexes();
	for (auto& h : hexes)
		if (h.second->IsOwnedBy(team)) // TODO: Check ships.
			game.GetMap().GetEmptyNeighbours(h.first, team.HasTech(TechType::WormholeGen), positions);

	m_positions.insert(m_positions.begin(), positions.begin(), positions.end());
}

void ExploreCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseExplorePos(m_positions, m_iPhase > 0), GetPlayer(game));
}

CmdPtr ExploreCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	if (dynamic_cast<const Input::CmdAbort*>(&msg))
		return nullptr;

	auto& m = VerifyCastInput<const Input::CmdExplorePos>(msg);
	VerifyInput("ExploreCmd::Process: invalid pos index", InRange(m_positions, m.m_iPos));
	m_iPos = m.m_iPos;

	const MapPos& pos = m_positions[m_iPos];

	ExploreRecord* pRec = new ExploreRecord(m_colour, pos.GetRing());
	DoRecord(RecordPtr(pRec), session);
	m_idHex = pRec->GetHexID();

	std::vector<int> hexIDs;
	hexIDs.push_back(m_idHex);
	return CmdPtr(new ExploreHexCmd(m_colour, session.GetGame(), pos, hexIDs, m_iPhase));
}

void ExploreCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveCntr("positions", m_positions, Serial::TypeSaver());
	node.SaveType("hex_id", m_idHex);
	node.SaveType("pos_index", m_iPos);
}

void ExploreCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadCntr("positions", m_positions, Serial::TypeLoader());
	node.LoadType("hex_id", m_idHex);
	node.LoadType("pos_index", m_iPos);
}

REGISTER_DYNAMIC(ExploreCmd)

//-----------------------------------------------------------------------------

class ExploreHexRecord : public TeamRecord
{
public:
	ExploreHexRecord() : m_idHex(-1), m_iRot(-1), m_bInfluence(false), m_discovery(DiscoveryType::None) {}

	ExploreHexRecord(Colour colour, const MapPos& pos, int idHex, int iRot, bool bInfluence) :
		TeamRecord(colour), m_pos(pos), m_idHex(idHex), m_iRot(iRot), m_bInfluence(bInfluence), m_discovery(DiscoveryType::None) {}

	DiscoveryType GetDiscovery() const { return m_discovery; }

	virtual void Apply(bool bDo, Game& game, const Controller& controller) override
	{
		if (bDo)
		{
			Hex& hex = game.GetMap().AddHex(m_pos, m_idHex, m_iRot);

			if (m_bInfluence)
			{
				hex.SetColour(m_colour);

				Team& team = game.GetTeam(m_colour);
				team.GetInfluenceTrack().RemoveDiscs(1);
				controller.SendMessage(Output::UpdateInfluenceTrack(team), game);

				m_discovery = hex.GetDiscoveryTile();
				if (m_discovery != DiscoveryType::None)
					hex.RemoveDiscoveryTile();

			}
		}
		else
		{
			if (m_bInfluence)
			{
				Team& team = game.GetTeam(m_colour);
				team.GetInfluenceTrack().AddDiscs(1);
				controller.SendMessage(Output::UpdateInfluenceTrack(team), game);

				if (m_discovery != DiscoveryType::None)
					game.GetMap().GetHex(m_pos).SetDiscoveryTile(m_discovery);
			}
			game.GetMap().DeleteHex(m_pos);
		}
		controller.SendMessage(Output::UpdateMap(game), game);
	}

	virtual void Save(Serial::SaveNode& node) const override 
	{
		__super::Save(node);
		node.SaveEnum("discovery", m_discovery);
		node.SaveType("pos", m_pos);
		node.SaveType("hex_id", m_idHex);
		node.SaveType("rotation", m_iRot);
		node.SaveType("influence", m_bInfluence);
	}
	
	virtual void Load(const Serial::LoadNode& node) override 
	{
		__super::Load(node);
		node.LoadEnum("discovery", m_discovery);
		node.LoadType("pos", m_pos);
		node.LoadType("hex_id", m_idHex);
		node.LoadType("rotation", m_iRot);
		node.LoadType("influence", m_bInfluence);
	}

	virtual std::string GetTeamMessage() const
	{
		const char* format = m_bInfluence ? "placed and influenced hex %0" : "placed hex %0";
		return FormatString(format, m_idHex);
	}

private:
	MapPos m_pos;
	int m_idHex, m_iRot;
	bool m_bInfluence;
	DiscoveryType m_discovery;
};

REGISTER_DYNAMIC(ExploreHexRecord)

//-----------------------------------------------------------------------------

class DiscoverAndExploreCmd : public DiscoverCmd
{
public:
	DiscoverAndExploreCmd() {}
	DiscoverAndExploreCmd(Colour colour, const LiveGame& game, DiscoveryType discovery) : DiscoverCmd(colour, game, discovery){}
private:
	virtual CmdPtr GetNextCmd(const LiveGame& game) const override { return CmdPtr(new ExploreCmd(m_colour, game, 1)); }
};

REGISTER_DYNAMIC(DiscoverAndExploreCmd)

//-----------------------------------------------------------------------------

ExploreHexCmd::ExploreHexCmd(Colour colour, const LiveGame& game, const MapPos& pos, std::vector<int> hexIDs, int iPhase) : 
	PhaseCmd(colour, iPhase), m_pos(pos), m_hexIDs(hexIDs), m_iRot(-1), m_iHex(-1), m_bInfluence(false), 
	m_bTaken(false), m_discovery(DiscoveryType::None)
{
	auto& team = GetTeam(game);

	bool bWormholeGen = team.HasTech(TechType::WormholeGen);

	std::vector<const Hex*> hexes = game.GetMap().GetSurroundingHexes(m_pos, team);

	Race race(team.GetRace());

	m_hexChoices.clear();
	
	for (int id : hexIDs)
	{
		Hex hex(id, pos, 0);

		bool bCanInfluenceHex = hex.GetShips().empty(); // Any ships must be ancients.
		HexChoice hc(id, bCanInfluenceHex && team.GetInfluenceTrack().GetDiscCount() > 0);
		
		EdgeSet inner = hex.GetWormholes();

		for (int i = 0; i < 6; ++i) // Try each rotation.
		{
			bool bMatch = false;
			for (auto e : EnumRange<Edge>())
			{
				const Hex* pHex = hexes[(int)e];
				
				bool bInner = inner[RotateEdge(e, -i)];
				bool bOuter = pHex && pHex->HasWormhole(ReverseEdge(e));

				if (bWormholeGen)
					bMatch = pHex && (bOuter || bInner);
				else
					bMatch = bOuter && bInner;

				if (bMatch)
					break;
			}
			if (bMatch)
				hc.m_rotations.push_back(i);
		}
		m_hexChoices.push_back(hc);
	}
}

void ExploreHexCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	bool bCanTake = Race(GetTeam(game).GetRace()).GetExploreChoices() > (int)m_hexChoices.size() 
		&& !game.GetHexBag(m_pos.GetRing()).IsEmpty();

	Output::ChooseExploreHex msg(m_pos.GetX(), m_pos.GetY(), bCanTake, game.GetActionPhase().CanRemoveCmd());
	for (auto& hc : m_hexChoices)
		msg.AddHexChoice(hc.m_idHex, hc.m_rotations, hc.m_bCanInfluence);

	controller.SendMessage(msg, GetPlayer(game));
}


CmdPtr ExploreHexCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	m_discovery = DiscoveryType::None;

	const LiveGame& game = session.GetGame();

	if (dynamic_cast<const Input::CmdExploreHexTake*>(&msg))
	{
		const HexBag& bag = game.GetHexBag(m_pos.GetRing());
		VerifyInput("ExploreHexCmd::Process: no tiles left in bag", !bag.IsEmpty());
		VerifyInput("ExploreHexCmd::Process: too many tiles taken", Race(GetTeam(game).GetRace()).GetExploreChoices() > (int)m_hexIDs.size());

		ExploreRecord* pRec = new ExploreRecord(m_colour, m_pos.GetRing());
		DoRecord(RecordPtr(pRec), session);
		int idHex = pRec->GetHexID();

		std::vector<int> hexIDs = m_hexIDs;
		hexIDs.insert(hexIDs.begin(), idHex); // At front so it appears first.
		m_bTaken = true;
		return CmdPtr(new ExploreHexCmd(m_colour, game, m_pos, hexIDs, m_iPhase));
	}

	bool bReject = !!dynamic_cast<const Input::CmdAbort*>(&msg);
	if (bReject)
	{
		// TODO: Add discarded hexes to discard pile. 
	}
	else
	{
		auto& m = VerifyCastInput<const Input::CmdExploreHex>(msg);

		VerifyInput("ExploreHexCmd::AcceptMessage", InRange(m_hexChoices, m.m_iHex));
		m_iHex = m.m_iHex;

		auto& hc = m_hexChoices[m_iHex];
	
		VerifyInput("ExploreHexCmd: rotation index", InRange(hc.m_rotations, m.m_iRot));
		m_iRot = m.m_iRot;

		ExploreHexRecord* pRec = new ExploreHexRecord(m_colour, m_pos, m_hexChoices[m_iHex].m_idHex, hc.m_rotations[m_iRot], m.m_bInfluence);
		DoRecord(RecordPtr(pRec), session);

		m_discovery = pRec->GetDiscovery();
	}

	const bool bFinish = m_iPhase + 1 == Race(GetTeam(game).GetRace()).GetExploreRate();

	if (m_discovery != DiscoveryType::None)
		return CmdPtr(bFinish ? new DiscoverCmd(m_colour, game, m_discovery) : new DiscoverAndExploreCmd(m_colour, game, m_discovery));

	return CmdPtr(bFinish ? nullptr : new ExploreCmd(m_colour, game, 1));
}

void ExploreHexCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveCntr("hex_ids", m_hexIDs, Serial::TypeSaver());
	node.SaveCntr("hex_choices", m_hexChoices, Serial::ClassSaver());
	node.SaveType("rot_index", m_iRot);
	node.SaveType("hex_index", m_iHex);
	node.SaveType("influence", m_bInfluence);
	node.SaveType("pos", m_pos);
	node.SaveType("taken", m_bTaken);
	node.SaveEnum("discovery", m_discovery);
}

void ExploreHexCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadCntr("hex_ids", m_hexIDs, Serial::TypeLoader());
	node.LoadCntr("hex_choices", m_hexChoices, Serial::ClassLoader());
	node.LoadType("rot_index", m_iRot);
	node.LoadType("hex_index", m_iHex);
	node.LoadType("influence", m_bInfluence);
	node.LoadType("pos", m_pos);
	node.LoadType("taken", m_bTaken);
	node.LoadEnum("discovery", m_discovery);
}

REGISTER_DYNAMIC(ExploreHexCmd)

//-----------------------------------------------------------------------------

void ExploreHexCmd::HexChoice::Save(Serial::SaveNode& node) const
{
	node.SaveType("hex_id", m_idHex);
	node.SaveType("can_influence", m_bCanInfluence);
	node.SaveCntr("rotations", m_rotations, Serial::TypeSaver());
}

void ExploreHexCmd::HexChoice::Load(const Serial::LoadNode& node)
{
	node.LoadType("hex_id", m_idHex);
	node.LoadType("can_influence", m_bCanInfluence);
	node.LoadCntr("rotations", m_rotations, Serial::TypeLoader());
}

