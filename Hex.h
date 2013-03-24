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
class Map;

enum class Edge { T, TR, BR, B, BL, TL };

class EdgeSet : public std::bitset<6>
{
public:
	EdgeSet(std::string s);
	bool operator[](Edge e) const;
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
	Hex(Map& map, int id, int nRotation);
	bool HasWormhole(Edge e) const;

	std::vector<Square*> GetAvailableSquares(const Team& team);
	Team* GetOwner() const { return m_pOwner; }
	void SetOwner(Team* pOwner);

	void AddShip(ShipType type, Team* pOwner);

private:
	void Init();

	int m_id;
	int m_nRotation; // [0, 5]
	std::vector<Square> m_squares;
	std::vector<Ship> m_ships;
	DiscoveryType m_discovery;
	EdgeSet m_wormholes;
	int m_nVictory;
	bool m_bArtifact;
	Map& m_map; 
	Team* m_pOwner;
	//structures
};

typedef std::unique_ptr<Hex> HexPtr;