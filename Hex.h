#pragma once

#include "Resources.h"
#include "MapPos.h"

#include <vector>
#include <bitset>
#include <memory>

enum class Resource;
enum class ShipType;
enum class DiscoveryType;
enum class TechType;
enum class Edge;
enum class Colour;
class Team;
class Game;
class HexDef;
class Blueprint;
class Map;

extern Edge ReverseEdge(Edge e);
extern Edge RotateEdge(Edge e, int n);

class EdgeSet;
class SquareDef;
class Hex;
enum class SquareType { Money, Science, Materials, Any, Orbital, _Count };

namespace Serial { class SaveNode; class LoadNode; }

// Proxy class - defers to Hex and SquareDef
class Square
{
public:
	Square(Hex& hex, int index);
	TechType GetRequiredTech() const;
	bool IsOccupied() const;
	void SetOccupied(bool b);
	SquareType GetType() const;

	int GetX() const;
	int GetY() const;

private:
	Hex& m_hex;
	int m_index;
	const SquareDef& GetDef() const;
};

class Ship
{
public:
	Ship();
	Ship(ShipType type, Colour owner) : m_type(type), m_colour(owner) {}
	ShipType GetType() const { return m_type; }
	Colour GetColour() const { return m_colour; }
	const Team* GetOwner(const Game& game) const;
	const Blueprint& GetBlueprint(const Game& game) const;

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	ShipType m_type;
	Colour m_colour;
};

class Hex
{
	friend class Square;
public:
	Hex();
	Hex(int id, const MapPos& pos, int nRotation);
	Hex(const Hex& rhs);
	bool HasWormhole(Edge e) const;

	std::vector<Square*> GetAvailableSquares(const Team& team);
	const std::vector<Square*> GetAvailableSquares(const Team& team) const { return const_cast<Hex*>(this)->GetAvailableSquares(team); }

	Colour GetColour() const { return m_colour; }
	void SetColour(Colour c);
	bool IsOwned() const;
	bool IsOwnedBy(const Team& team) const;

	void AddShip(ShipType type, Colour owner);
	void RemoveShip(ShipType type, Colour owner);
	bool HasShip(const Colour& c, ShipType ship) const;
	bool HasShip(const Colour& c, bool bMoveableOnly = false) const;
	bool HasEnemyShip(const Game& game, const Team* pTeam) const; // Ancients and their allies are not enemies.
	bool HasForeignShip(const Colour& c, bool bPlayerShipsOnly = false) const; // Ancients and their allies are foreign.
	std::set<Colour> GetShipColours(bool bPlayerShipsOnly = false) const;
	bool AreAllShipsPinned(const Colour& c) const;

	int GetID() const { return m_id; }
	const MapPos& GetPos() const { return m_pos; }
	void SetPos(const MapPos& pos) { m_pos = pos; }
	int GetRotation() const { return m_nRotation; }
	const std::vector<Square>& GetSquares() const { return m_squares; }
	const std::vector<Ship>& GetShips() const { return m_ships; }
	DiscoveryType GetDiscoveryTile() const { return m_discovery; }
	const EdgeSet& GetWormholes() const; // Non-rotated.
	int GetVictoryPoints() const;
	bool HasArtifact() const;
	bool HasDiscovery() const;
	void RemoveDiscoveryTile();
	void SetDiscoveryTile(DiscoveryType type);
	bool CanMoveOut(Colour c) const;
	bool CanMoveThrough(Colour c) const;
	
	Square* FindSquare(SquareType type, bool bOccupied);

	bool HasNeighbour(const Map& map, bool bWormholeGen) const;

	void SetOrbital(bool b) { m_bOrbital = b; }
	void SetMonolith(bool b) { m_bMonolith = b; }
	bool HasOrbital() const { return m_bOrbital; }
	bool HasMonolith() const { return m_bMonolith; }

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	const HexDef& GetDef() const { return *m_pDef; }

	void InitSquares();

	void SetSquareOccupied(int i, bool b);
	bool IsSquareOccupied(int i) const;

	int GetPinnage(const Colour& c) const;
	
	const HexDef* m_pDef; 
	int m_id;
	MapPos m_pos;
	int m_nRotation; // [0, 5]
	std::vector<Square> m_squares;
	std::vector<Ship> m_ships;
	DiscoveryType m_discovery;
	Colour m_colour;
	std::set<int> m_occupied;
	bool m_bOrbital, m_bMonolith;
};

typedef std::unique_ptr<Hex> HexPtr;

