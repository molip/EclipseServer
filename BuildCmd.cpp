#include "stdafx.h"
#include "BuildCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"

namespace
{
	Buildable ShipToBuildable(ShipType s)
	{
		switch (s)
		{
		case ShipType::Interceptor:	return Buildable::Interceptor;
		case ShipType::Cruiser:		return Buildable::Cruiser;
		case ShipType::Dreadnought:	return Buildable::Dreadnought;
		case ShipType::Starbase:	return Buildable::Starbase;
		}
		AssertThrow("ShipToBuildable", false);
		return Buildable::None;
	}

	ShipType BuildableToShip(Buildable b)
	{
		switch (b)
		{
		case Buildable::Interceptor:	return ShipType::Interceptor;
		case Buildable::Cruiser:		return ShipType::Cruiser;
		case Buildable::Dreadnought:	return ShipType::Dreadnought;
		case Buildable::Starbase:		return ShipType::Starbase;
		}
		AssertThrow("BuildableToShip", false);
		return ShipType::None;
	}
}

//-----------------------------------------------------------------------------

class BuildRecord : public Record
{
public:
	BuildRecord() : m_colour(Colour::None), m_buildable(Buildable::None) {}
	BuildRecord(Colour colour, const MapPos& pos, Buildable buildable) : m_colour(colour), m_pos(pos), m_buildable(buildable) {}

	virtual void Save(Serial::SaveNode& node) const override 
	{
		node.SaveEnum("colour", m_colour);
		node.SaveEnum("buildable", m_buildable);
		node.SaveType("pos", m_pos);
	}
	
	virtual void Load(const Serial::LoadNode& node) override 
	{
		node.LoadEnum("colour", m_colour);
		node.LoadEnum("buildable", m_buildable);
		node.LoadType("pos", m_pos);
	}

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) override
	{
		Team& team = game.GetTeam(m_colour);

		Hex& hex = game.GetMap().GetHex(m_pos);
		if (m_buildable == Buildable::Orbital)
			hex.SetOrbital(bDo);
		else if (m_buildable == Buildable::Monolith)
			hex.SetMonolith(bDo);
		else
		{
			ShipType s = BuildableToShip(m_buildable);
			if (bDo)
				hex.AddShip(s, m_colour);
			else
				hex.RemoveShip(s, m_colour);

			team.AddShips(s, bDo ? -1 : 1);
		}

		int cost = Race(team.GetRace()).GetBuildCost(m_buildable);

		team.GetStorage()[Resource::Materials] += bDo ? -cost : cost;

		controller.SendMessage(Output::UpdateMap(game), game);
		controller.SendMessage(Output::UpdateStorageTrack(team), game);
	}

	Colour m_colour;
	Buildable m_buildable;
	MapPos m_pos;
};

REGISTER_DYNAMIC(BuildRecord)

//-----------------------------------------------------------------------------

BuildCmd::BuildCmd(Colour colour, LiveGame& game, int iPhase) : PhaseCmd(colour, iPhase)
{
}

bool BuildCmd::CanAfford(const LiveGame& game, Buildable b) const
{
	return Race(GetTeam(game).GetRace()).GetBuildCost(b) <= GetTeam(game).GetStorage()[Resource::Materials];
}

void BuildCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	const Team& team = GetTeam(game);

	std::set<ShipType> ships;
	for (auto s : EnumRange<ShipType>())
		if (CanAfford(game, ShipToBuildable(s)) && team.GetUnusedShips(s) > 0)
			ships.insert(s);

	std::map<MapPos, std::pair<bool, bool>> hexes; // (orbital, monolith)

	for (auto& h : game.GetMap().GetHexes()) // Pos, hex.
		if (h.second->GetColour() == m_colour)
			hexes[h.first] = std::make_pair(!h.second->HasOrbital() && CanAfford(game, Buildable::Orbital) && team.HasTech(TechType::Orbital), 
											!h.second->HasMonolith() && CanAfford(game, Buildable::Monolith) && team.HasTech(TechType::Monolith));

	controller.SendMessage(Output::ChooseBuild(ships, hexes, m_iPhase > 0), GetPlayer(game));
}

CmdPtr BuildCmd::Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game)
{
	if (dynamic_cast<const Input::CmdAbort*>(&msg))
		return nullptr;

	auto& m = CastThrow<const Input::CmdBuild>(msg);
	MapPos pos(m.m_x, m.m_y);
	bool bOrbital = m.m_buildable == Buildable::Orbital;
	bool bMonolith = m.m_buildable == Buildable::Monolith;

	Team& team = GetTeam(game);
	Hex& hex = game.GetMap().GetHex(pos);
	AssertThrow("BuildCmd::Process: invalid hex", hex.GetColour() == m_colour);
	AssertThrow("BuildCmd::Process: not enough materials", CanAfford(game, m.m_buildable));
	AssertThrow("BuildCmd::Process: already got orbital", !bOrbital || !hex.HasOrbital());
	AssertThrow("BuildCmd::Process: already got monolith", !bMonolith || !hex.HasMonolith());
	AssertThrow("BuildCmd::Process: can't build orbitals", !bOrbital || team.HasTech(TechType::Orbital));
	AssertThrow("BuildCmd::Process: can't build monoliths", !bMonolith || team.HasTech(TechType::Monolith));
	AssertThrow("BuildCmd::Process: no ships left", bMonolith || bOrbital || 
		GetTeam(game).GetUnusedShips(BuildableToShip(m.m_buildable)) > 0);

	BuildRecord* pRec = new BuildRecord(m_colour, pos, m.m_buildable);
	pRec->Do(game, controller);
	game.PushRecord(RecordPtr(pRec));

	if (!team.HasPassed() && m_iPhase + 1 < Race(team.GetRace()).GetBuildRate())
		return CmdPtr(new BuildCmd(m_colour, game, m_iPhase + 1));

	return nullptr;
}

REGISTER_DYNAMIC(BuildCmd)
