#pragma once

#include "MapPos.h"
#include "Hex.h"

#include <memory>
#include <vector>
#include <map>
#include <set>

class Game;
class MapPos;

namespace Serial { class SaveNode; class LoadNode; }

class Map
{
public:
	Map(Game& game);
	Map(const Map& rhs, Game& game);

	Hex& AddHex(const MapPos& pos, int id, int rotation);
	void DeleteHex(const MapPos& pos);
	std::vector<MapPos> GetTeamStartPositions() const;
	
	Game& GetGame() { return m_game; }

	typedef std::map<MapPos, HexPtr> HexMap;
	const HexMap& GetHexes() const { return m_hexes; }
	
	Hex& GetHex(const MapPos& pos);
	Hex* FindHex(const MapPos& pos);
	
	const Hex& GetHex(const MapPos& pos) const { return const_cast<Map*>(this)->GetHex(pos); }
	const Hex* FindHex(const MapPos& pos) const { return const_cast<Map*>(this)->FindHex(pos); }
	
	Hex* FindHex(int hexId);
	const Hex* FindHex(int hexId) const { return const_cast<Map*>(this)->FindHex(hexId); }
	const Hex* FindPendingBattleHex(const Game& game) const;

	void GetInfluencableNeighbours(const MapPos& pos, const Team& team, std::set<MapPos>& neighbours) const;
	void GetEmptyNeighbours(const MapPos& pos, bool bHasWormholeGen, std::set<MapPos>& neighbours) const;
	std::vector<const Hex*> GetValidExploreOriginNeighbours(const MapPos& pos, const Team& team) const; // Returns 6 hexes, including nulls.
	std::set<MapPos> GetNeighbours(const MapPos& pos, bool bHasWormholeGen) const;
	bool HasNeighbour(const MapPos& pos, bool bWormholeGen) const;

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	HexMap m_hexes;
	Game& m_game;
};
