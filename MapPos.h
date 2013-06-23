#pragma once

#include"App.h"

class Game;
enum class HexRing;

enum class Edge { T, TR, BR, B, BL, TL, _Count };

class MapPos
{
	friend std::ostream & operator<< (std::ostream &lhs, const MapPos& rhs);
	friend std::istream& operator >>(std::istream &lhs, MapPos& rhs);

public:
	MapPos() : m_x(0), m_y(0) {}
	MapPos(int x, int y) : m_x(x), m_y(y) {}
	
	MapPos GetNeighbour(Edge e) const;
	int GetDist(const MapPos& rhs) const;
	bool operator <(const MapPos& rhs) const;
	bool operator ==(const MapPos& rhs) const { return m_x == rhs.m_x && m_y == rhs.m_y; }
	bool operator !=(const MapPos& rhs) const { return !operator ==(rhs); }

	int GetX() const { return m_x; }
	int GetY() const { return m_y; }
	int GetZ() const { return -(m_x + m_y); }

	bool IsCentre() const { return m_x == 0 && m_y == 0; }
	HexRing GetRing() const;

private:
	int m_x; // Increasing SE
	int m_y; // Increasing S
};

DEFINE_UNIQUE_PTR(MapPos)