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
	Team* GetOwner() const { return m_pOwner; }
	void SetOwner(Team* pOwner);
	SquareType GetType() const { return m_type; }

	int GetX() const { return m_x; }
	int GetY() const { return m_y; }

private:
	int m_x, m_y; // Centre (pixels from TL of hex image).
	SquareType m_type;
	bool m_bAdvanced;
	Team* m_pOwner;
};

class Ship
{
public:
	Ship(ShipType type, Team* pOwner = nullptr) : m_type(type), m_pOwner(pOwner) {}
	ShipType GetType() const { return m_type; }
	Team* GetOwner() const { return m_pOwner; }

private:
	ShipType m_type;
	Team* m_pOwner;
};

class Hex
{
public:
	Hex(Game* pGame, int id, const MapPos& pos, int nRotation);
	bool HasWormhole(Edge e) const;

	std::vector<Square*> GetAvailableSquares();
	Team* GetOwner() const { return m_pOwner; }
	void SetOwner(Team* pOwner);

	void AddShip(ShipType type, Team* pOwner);
	bool HasShip(const Team* pOwner) const;
	bool HasEnemyShip(const Team* pTeam) const;

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
	Team* m_pOwner;
	//structures
};

typedef std::unique_ptr<Hex> HexPtr;

