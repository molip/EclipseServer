#include "stdafx.h"
#include "MapPos.h"
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
	else if (e == Edge::B || e == Edge::BL)
		dy = 1;

	if (e == Edge::TR || e == Edge::BR)
		dx = 1;
	else if (e == Edge::BL || e == Edge::TL)
		dx = -1;

	return MapPos(m_x + dx, m_y + dy);
}

int MapPos::GetDist(const MapPos& rhs) const
{
	return std::max(std::max(abs(m_x - rhs.m_x), abs(m_y - rhs.m_y)), abs(GetZ() - rhs.GetZ()));
}

bool MapPos::operator <(const MapPos& rhs) const 
{ 
	if (m_x != rhs.m_x)
		return m_x < rhs.m_x;
	
	return m_y < rhs.m_y; 
}

HexRing MapPos::GetRing() const
{
	VERIFY_MODEL(!IsCentre());

	switch (GetDist(MapPos(0, 0)))
	{
	case 1: return HexRing::Inner;
	case 2: return HexRing::Middle;
	}

	return HexRing::Outer;
}

std::ostream& operator <<(std::ostream &lhs, const MapPos& rhs)
{
	lhs << rhs.m_x << " " << rhs.m_y;
	return lhs;
}

std::istream& operator >>(std::istream &lhs, MapPos& rhs)
{
	lhs >> rhs.m_x >> rhs.m_y;
	return lhs;
}
