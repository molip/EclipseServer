#pragma once

#include "Resources.h"

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

enum class Edge { T, TR, BR, B, BL, TL, _Count };

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

class Square
{
public:
	Square(int x = 0, int y = 0, Resource type = Resource::Any, bool bAdvanced = false);
	TechType GetRequiredTech() const;
	Team* GetOwner() const { return m_pOwner; }
	void SetOwner(Team* pOwner);
	Resource GetType() const { return m_type; }

private:
	int m_x, m_y; // Centre (pixels from TL of hex image).
	Resource m_type;
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
	Hex(Game* pGame, int id, int nRotation);
	bool HasWormhole(Edge e) const;

	std::vector<Square*> GetAvailableSquares(const Team& team);
	Team* GetOwner() const { return m_pOwner; }
	void SetOwner(Team* pOwner);

	void AddShip(ShipType type, Team* pOwner);
	bool HasShip(const Team* pOwner) const;
	bool HasEnemyShip(const Team* pTeam) const;

	int GetID() const { return m_id; }
	int GetRotation() const { return m_nRotation; }
	const std::vector<Square>& GetSquares() const { return m_squares; }
	const std::vector<Ship>& GetShips() const { return m_ships; }
	DiscoveryType GetDiscoveryTile() const { return m_discovery; }
	EdgeSet GetWormholes() const { return m_wormholes; } // Non-rotated.
	int GetVictoryPoints() const { return m_nVictory; }
	bool HasArtifact() const { return m_bArtifact; }

private:
	void Init(Game* pGame);

	int m_id;
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

