#pragma once

#include "Hex.h"

class Game;
enum class HexRing;

class MapPos
{
public:
	MapPos() : m_x(0), m_y(0) {}
	MapPos(int x, int y) : m_x(x), m_y(y) {}
	
	MapPos GetNeighbour(Edge e) const;
	int GetDist(const MapPos& rhs) const;
	bool operator <(const MapPos& rhs) const;

	int GetX() const { return m_x; }
	int GetY() const { return m_y; }
	int GetZ() const { return -(m_x + m_y); }

	bool IsCentre() const { return m_x == 0 && m_y == 0; }
	HexRing GetRing() const;

private:
	int m_x; // Increasing SE
	int m_y; // Increasing S
};
