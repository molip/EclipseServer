#pragma once

#include "MapPos.h"

#include <memory>
#include <vector>
#include <map>
#include <set>

class Game;

class Map
{
public:
	Map(Game& game);

	Hex& AddHex(const MapPos& pos, int id, int rotation);
	void DeleteHex(const MapPos& pos);
	std::vector<MapPos> GetTeamStartPositions() const;
	
	Game& GetGame() { return m_game; }

	typedef std::map<MapPos, HexPtr> HexMap;
	const HexMap& GetHexes() const { return m_hexes; }
	void GetEmptyNeighbours(const MapPos& pos, bool bHasWormholeGen, std::set<MapPos>& neighbours) const;
	std::vector<const Hex*> GetSurroundingHexes(const MapPos& pos, const Team& team) const; // Returns empty hexes too.

private:
	Hex* GetHex(const MapPos& pos);
	const Hex* GetHex(const MapPos& pos) const;

	HexMap m_hexes;
	Game& m_game;
};
