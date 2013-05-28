#include "stdafx.h"
#include "Map.h"
#include "Hex.h"
#include "Game.h"
#include "App.h"

Map::Map(Game& game) : m_game(game)
{
	AddHex(MapPos(0, 0), 001, 0);
}

Hex* Map::GetHex(const MapPos& pos)
{
	return const_cast<Hex*>(const_cast<const Map*>(this)->GetHex(pos));
}

const Hex* Map::GetHex(const MapPos& pos) const
{
	auto h = m_hexes.find(pos);
	return h == m_hexes.end() ? nullptr : h->second.get();
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
	AssertThrowModel("Map::AddHex: hex already occupied", GetHex(pos) == nullptr);
	AssertThrowModel("Map::AddHex: invalid rotation", rotation >= 0 && rotation < 6);
	Hex* p = new Hex(&m_game, id, pos, rotation);
	m_hexes.insert(std::make_pair(pos, HexPtr(p)));
	return *p;
}

void Map::DeleteHex(const MapPos& pos)
{
	bool bOK = m_hexes.erase(pos) == 1;
	AssertThrowModel("Map::DeleteHex", bOK);
}

void Map::GetInfluencableNeighbours(const MapPos& pos, const Team& team, std::set<MapPos>& neighbours) const
{
	const bool bWormholeGen = team.HasTech(TechType::WormholeGen);

	const Hex* pHex = GetHex(pos);
	AssertThrow("Map::GetInfluencableNeighbours: invalid position", !!pHex);
	AssertThrow("Map::GetInfluencableNeighbours: wrong owner", !pHex->IsOwned() || pHex->IsOwnedBy(team));

	for (auto e : EnumRange<Edge>())
		if (int nWormholes = pHex->HasWormhole(e) + bWormholeGen)
		{
			MapPos pos2 = pos.GetNeighbour(e);
			if (const Hex* pHex2 = GetHex(pos2))
				if (!pHex2->IsOwned()) // "a hex that does not contain an Influence Disc..."
					if (nWormholes + pHex2->HasWormhole(ReverseEdge(e)) >= 2)
						if (!pHex2->HasForeignShip(team.GetGame(), &team)) // "...or an enemy Ship"
							neighbours.insert(pos2);
		}
}

void Map::GetEmptyNeighbours(const MapPos& pos, bool bWormholeGen, std::set<MapPos>& neighbours) const
{
	const Hex* pHex = GetHex(pos);
	AssertThrow("Map::GetEmptyNeighbours: invalid position", !!pHex);

	for (auto e : EnumRange<Edge>())
		if (bWormholeGen || pHex->HasWormhole(e))
		{
			MapPos pos2 = pos.GetNeighbour(e);
			if (GetHex(pos2) == nullptr)
				if (!m_game.GetHexBag(pos2.GetRing()).IsEmpty())
					neighbours.insert(pos2);
		}
}

std::vector<const Hex*> Map::GetSurroundingHexes(const MapPos& pos, const Team& team) const
{
	std::vector<const Hex*> hexes;
	hexes.resize(6);

	for (auto e : EnumRange<Edge>())
		if (const Hex* pHex = GetHex(pos.GetNeighbour(e)))
			if (pHex->IsOwnedBy(team)) // TODO: Check ships.
				hexes[(int)e] = pHex;

	return hexes;		
}
