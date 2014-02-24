#include "stdafx.h"
#include "Map.h"
#include "Hex.h"
#include "Game.h"
#include "App.h"
#include "EdgeSet.h"
#include "Serial.h"

Map::Map(Game& game) : m_game(game)
{
}

Map::Map(const Map& rhs, Game& game) : m_game(game)
{
	for (auto& h : rhs.m_hexes)
		m_hexes.insert(std::make_pair(h.first, HexPtr(new Hex(*h.second))));
}

Hex* Map::FindHex(const MapPos& pos)
{
	auto h = m_hexes.find(pos);
	return h == m_hexes.end() ? nullptr : h->second.get();
}

Hex& Map::GetHex(const MapPos& pos)
{
	Hex* pHex = FindHex(pos);
	AssertThrowModel("Map::GetHex", !!pHex);
	return *pHex;
}

std::vector<MapPos> Map::GetTeamStartPositions() const
{
	const int nTeams = m_game.GetTeams().size();
	AssertThrowModel("Map::GetTeamStartPositions", nTeams > 0 && nTeams < 7);

	std::string s;
	switch (nTeams)
	{
	case 6: s = "111111"; break;
	case 5: s = "111011"; break;
	case 4: s = "011011"; break;
	case 3: s = "101010"; break;
	case 2: s = "100100"; break;
	case 1: s = "100000"; break;
	}

	std::vector<Edge> edges = EdgeSet(s).GetEdges();
	assert(edges.size() == nTeams);

	std::vector<MapPos> positions;
	for (int i = 0; i < nTeams; ++i)
	{
		Edge e = edges[i];
		positions.push_back(MapPos(0, 0).GetNeighbour(e).GetNeighbour(e));
	}
	return positions;
}

Hex& Map::AddHex(const MapPos& pos, int id, int rotation)
{
	AssertThrowModel("Map::AddHex: hex already occupied", FindHex(pos) == nullptr);
	AssertThrowModel("Map::AddHex: invalid rotation", rotation >= 0 && rotation < 6);
	Hex* p = new Hex(id, pos, rotation);
	m_hexes.insert(std::make_pair(pos, HexPtr(p)));
	
	if (p->HasDiscovery())
		p->SetDiscoveryTile(m_game.GetDiscoveryBag().TakeTile());

	return *p;
}

void Map::DeleteHex(const MapPos& pos)
{
	auto i = m_hexes.find(pos);
	AssertThrowModel("Map::DeleteHex: hex not found", i != m_hexes.end());

	if (i->second->HasDiscovery())
	{
		DiscoveryType d = i->second->GetDiscoveryTile();
		AssertThrowModel("Map::DeleteHex: discovery tile has gone", d != DiscoveryType::None);
		m_game.GetDiscoveryBag().ReturnTile(d);
	}
	m_hexes.erase(i);
}

void Map::GetInfluencableNeighbours(const MapPos& pos, const Team& team, std::set<MapPos>& neighbours) const
{
	const bool bWormholeGen = team.HasTech(TechType::WormholeGen);

	const Hex& hex= GetHex(pos);
	AssertThrow("Map::GetInfluencableNeighbours: wrong owner", !hex.IsOwned() || hex.IsOwnedBy(team));

	for (auto e : EnumRange<Edge>())
		if (int nWormholes = hex.HasWormhole(e) + bWormholeGen)
		{
			MapPos pos2 = pos.GetNeighbour(e);
			if (const Hex* pHex2 = FindHex(pos2))
				if (!pHex2->IsOwned()) // "a hex that does not contain an Influence Disc..."
					if (nWormholes + pHex2->HasWormhole(ReverseEdge(e)) >= 2)
						if (!pHex2->HasForeignShip(m_game, &team)) // "...or an enemy Ship"
							neighbours.insert(pos2);
		}
}

void Map::GetEmptyNeighbours(const MapPos& pos, bool bWormholeGen, std::set<MapPos>& neighbours) const
{
	const Hex& hex = GetHex(pos);

	for (auto e : EnumRange<Edge>())
		if (bWormholeGen || hex.HasWormhole(e))
		{
			MapPos pos2 = pos.GetNeighbour(e);
			if (FindHex(pos2) == nullptr)
				if (!m_game.GetHexBag(pos2.GetRing()).IsEmpty())
					neighbours.insert(pos2);
		}
}

std::set<MapPos> Map::GetNeighbours(const MapPos& pos, bool bWormholeGen) const
{
	std::set<MapPos> neighbours;

	const Hex& hex = GetHex(pos);

	for (auto e : EnumRange<Edge>())
		if (bWormholeGen || hex.HasWormhole(e))
		{
			MapPos pos2 = pos.GetNeighbour(e);
			if (FindHex(pos2))
				neighbours.insert(pos2);
		}
	return neighbours;
}

bool Map::HasNeighbour(const MapPos& pos, bool bWormholeGen) const
{
	const Hex& hex = GetHex(pos);

	for (auto e : EnumRange<Edge>())
		if (bWormholeGen || hex.HasWormhole(e))
		{
			MapPos pos2 = pos.GetNeighbour(e);
			if (FindHex(pos2))
				return true;
		}
	return false;
}

std::vector<const Hex*> Map::GetSurroundingHexes(const MapPos& pos, const Team& team) const
{
	std::vector<const Hex*> hexes;
	hexes.resize(6);

	for (auto e : EnumRange<Edge>())
		if (const Hex* pHex = FindHex(pos.GetNeighbour(e)))
			if (pHex->IsOwnedBy(team)) // TODO: Check ships.
				hexes[(int)e] = pHex;

	return hexes;		
}

void Map::Save(Serial::SaveNode& node) const
{
	node.SaveMap("hexes", m_hexes, Serial::TypeSaver(), Serial::ClassPtrSaver());
}

void Map::Load(const Serial::LoadNode& node)
{
	node.LoadMap("hexes", m_hexes, Serial::TypeLoader(), Serial::ClassPtrLoader());

	for (auto& kv : m_hexes)
		kv.second->SetPos(kv.first);
}
