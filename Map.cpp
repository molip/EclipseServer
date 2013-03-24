#include "stdafx.h"
#include "Map.h"
#include "Hex.h"
#include "Game.h"
#include "App.h"

/*
0,0
	1,0		
0,1		2,0
	1,1		3,0
0,2		2,1		4,0
*/

MapPos MapPos::GetNeighbour(Edge e) const
{
	int dx = 0, dy = 0;
	
	if (e == Edge::T || e == Edge::TR)
		dy = -1;
	else if (e == Edge::BR || e == Edge::B)
		dy = 1;

	if (e == Edge::TR || e == Edge::BR)
		dx = 1;
	else if (e == Edge::BL || e == Edge::TL)
		dx = -1;

	return MapPos(m_x + dx, m_y + dy);
}

int MapPos::GetDist(const MapPos& rhs) const
{
	return std::max(std::max(m_x - rhs.m_x, m_y - rhs.m_y), GetZ() - rhs.GetZ());
}

bool MapPos::operator <(const MapPos& rhs) const 
{ 
	return m_x < rhs.m_x || m_y < rhs.m_y; 
}

//-----------------------------------------------------------------------------

Map::Map(Game& game) : m_game(game)
{
	AddHex(MapPos(0, 0), 001, 0);
}

Hex* Map::GetHex(const MapPos& pos)
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
	Hex* p = new Hex(*this, id,  rotation);
	m_hexes.insert(std::make_pair(pos, HexPtr(p)));
	return *p;
}

//HexRing Map::GetHexRing(const MapPos& pos) const 
//{
//}
