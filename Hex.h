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

extern Edge ReverseEdge(Edge e);
extern Edge RotateEdge(Edge e, int n);

class EdgeSet;
class SquareDef;
class Hex;
enum class SquareType { Money, Science, Materials, Any, Orbital, _Count };

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
	Ship(ShipType type, Colour owner) : m_type(type), m_colour(owner) {}
	ShipType GetType() const { return m_type; }
	Colour GetColour() const { return m_colour; }
	const Team* GetOwner(const Game& game) const;

private:
	ShipType m_type;
	Colour m_colour;
};

class Hex
{
	friend class Square;
public:
	Hex(int id, const MapPos& pos, int nRotation);
	Hex(const Hex& rhs);
	bool HasWormhole(Edge e) const;

	std::vector<Square*> GetAvailableSquares(const Team& team);
	Colour GetColour() const { return m_colour; }
	void SetColour(Colour c);
	bool IsOwned() const;
	bool IsOwnedBy(const Team& team) const;

	void AddShip(ShipType type, Colour owner);
	bool HasShip(const Team* pTeam) const;
	bool HasEnemyShip(const Game& game, const Team* pTeam) const; // Ancients and their allies are not enemies.
	bool HasForeignShip(const Game& game, const Team* pTeam) const; // Ancients and their allies are foreign.

	int GetID() const { return m_id; }
	const MapPos& GetPos() const { return m_pos; }
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
	
	Square* FindSquare(SquareType type, bool bOccupied);

private:
	const HexDef& GetDef() const { return *m_pDef; }

	void Init();

	void SetSquareOccupied(int i, bool b) { m_occupied[i] = b; }
	bool IsSquareOccupied(int i) const { return m_occupied[i]; }

	const HexDef* m_pDef; 
	int m_id;
	const MapPos m_pos;
	int m_nRotation; // [0, 5]
	std::vector<Square> m_squares;
	std::vector<Ship> m_ships;
	DiscoveryType m_discovery;
	Colour m_colour;
	std::vector<bool> m_occupied;
	//structures
};

typedef std::unique_ptr<Hex> HexPtr;

