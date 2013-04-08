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
	const int nTeams = m_game.GetTeamOrder().size();
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
	Hex* p = new Hex(&m_game, id,  rotation);
	m_hexes.insert(std::make_pair(pos, HexPtr(p)));
	return *p;
}

void Map::GetEmptyNeighbours(const MapPos& pos, bool bWormholeGen, std::set<MapPos>& neighbours) const
{
	const Hex* pHex = GetHex(pos);
	AssertThrow("Map::GetEmptyNeighbours: invalid position", !!pHex);

	for (auto e : EnumRange<Edge>())
		if (bWormholeGen || pHex->HasWormhole(e))
		{
			MapPos pos2 = pos.GetNeighbour(e);
			if (!m_game.GetHexBag(pos2.GetRing()).IsEmpty())
				if (GetHex(pos2) == nullptr)
					neighbours.insert(pos2);
		}
}

std::vector<const Hex*> Map::GetSurroundingHexes(const MapPos& pos, const Team& team) const
{
	std::vector<const Hex*> hexes;
	hexes.resize(6);

	for (auto e : EnumRange<Edge>())
		if (const Hex* pHex = GetHex(pos.GetNeighbour(e)))
			if (pHex->GetOwner() == &team) // TODO: Check ships.
				hexes[(int)e] = pHex;

	return hexes;		
}
