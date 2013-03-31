#pragma once

#include "Hex.h"

#include <memory>
#include <vector>
#include <map>

class Game;

class MapPos
{
public:
	MapPos(int x, int y) : m_x(x), m_y(y) {}
	
	MapPos GetNeighbour(Edge e) const;
	int GetDist(const MapPos& rhs) const;
	bool operator <(const MapPos& rhs) const;

	int GetX() const { return m_x; }
	int GetY() const { return m_y; }
	int GetZ() const { return -(m_x + m_y); }

private:
	int m_x; // Increasing SE
	int m_y; // Increasing S
};

class Map
{
public:
	Map(Game& game);

	Hex& AddHex(const MapPos& pos, int id, int rotation);
	std::vector<MapPos> GetTeamStartPositions() const;
	
	Game& GetGame() { return m_game; }

	typedef std::map<MapPos, std::unique_ptr<Hex>> HexMap;
	const HexMap& GetHexes() const { return m_hexes; }

	//HexRing GetHexRing(const MapPos& pos) const;

private:
	Hex* GetHex(const MapPos& pos);

	HexMap m_hexes;
	Game& m_game;
};
