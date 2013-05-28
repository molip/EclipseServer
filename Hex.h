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

extern Edge ReverseEdge(Edge e);
extern Edge RotateEdge(Edge e, int n);

class EdgeSet : public std::bitset<6>
{
public:
	EdgeSet() {}
	EdgeSet(std::string s);
	bool operator[](Edge e) const;
	EdgeSet::reference operator[](Edge e);
	std::vector<Edge> GetEdges() const;
};

enum class SquareType { Money, Science, Materials, Any, Orbital, _Count };

class Square
{
public:
	Square(int x = 0, int y = 0, SquareType type = SquareType::Any, bool bAdvanced = false);
	TechType GetRequiredTech() const;
	bool IsOccupied() const { return m_bOccupied; }
	void SetOccupied(bool b) { m_bOccupied = b; }
	SquareType GetType() const { return m_type; }

	int GetX() const { return m_x; }
	int GetY() const { return m_y; }

private:
	int m_x, m_y; // Centre (pixels from TL of hex image).
	SquareType m_type;
	bool m_bAdvanced;
	bool m_bOccupied;
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
public:
	Hex(Game* pGame, int id, const MapPos& pos, int nRotation);
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
	EdgeSet GetWormholes() const { return m_wormholes; } // Non-rotated.
	int GetVictoryPoints() const { return m_nVictory; }
	bool HasArtifact() const { return m_bArtifact; }

	void RemoveDiscoveryTile();

private:
	void Init(Game* pGame);

	int m_id;
	const MapPos m_pos;
	int m_nRotation; // [0, 5]
	std::vector<Square> m_squares;
	std::vector<Ship> m_ships;
	DiscoveryType m_discovery;
	EdgeSet m_wormholes;
	int m_nVictory;
	bool m_bArtifact;
	Colour m_colour;
	//structures
};

typedef std::unique_ptr<Hex> HexPtr;

