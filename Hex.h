#pragma once

#include "Resources.h"
#include "MapPos.h"
#include "Types.h"

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



// Proxy class - defers to Hex and SquareDef
class Square
{
public:
	Square(Hex& hex, int index);
	bool operator==(const Square& rhs) const { return m_index == rhs.m_index; }
	TechType GetRequiredTech() const;
	bool CanOccupy(const Team& team) const;
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
	static const Blueprint& GetBlueprint(Colour colour, ShipType type, const Game& game);
};

class Squadron 
{
	friend class Fleet;
public:
	Squadron();
	Squadron(ShipType type);
	bool operator==(const Squadron& rhs) const;
		
	ShipType GetType() const { return m_type; }
	const Blueprint& GetBlueprint(const Game& game) const;
	int GetShipCount() const { return m_shipCount; }

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);
private:
	ShipType m_type; 
	int m_shipCount;
	Colour m_colour; // Not saved. 
};

class Fleet
{
	friend class Hex;
public:
	Fleet();
	Fleet(Colour colour);
	bool operator==(const Fleet& rhs) const;

	Colour GetColour() const { return m_colour; }
	const Team* GetOwner(const Game& game) const;
	const Squadron* FindSquadron(ShipType type) const { return const_cast<Fleet*>(this)->FindSquadron(type); }
	const std::vector<Squadron>& GetSquadrons() const { return m_squadrons; }
	int GetShipCount() const;

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);
private:
	void AddShip(ShipType type);
	void RemoveShip(ShipType type);
	Squadron* FindSquadron(ShipType type);
	
	std::vector<Squadron> m_squadrons;
	Colour m_colour;
};

class Hex
{
	friend class Square;
public:
	Hex();
	Hex(int id, const MapPos& pos, int nRotation);
	Hex(const Hex& rhs);

	bool operator==(const Hex& rhs) const;
	bool operator!=(const Hex& rhs) const { return !(*this == rhs); }

	bool HasWormhole(Edge e) const;

	std::vector<Square*> GetAvailableSquares(const Team& team);
	const std::vector<Square*> GetAvailableSquares(const Team& team) const { return const_cast<Hex*>(this)->GetAvailableSquares(team); }

	SquareCounts GetAvailableSquareCounts(const Team& team) const;
	SquareCounts GetOccupiedSquareCounts() const;
	bool HasPopulation() const;

	Colour GetColour() const { return m_colour; }
	void SetColour(Colour c);
	bool IsOwned() const;
	bool IsOwnedBy(const Team& team) const;

	Fleet* FindFleet(Colour c);
	const Fleet* FindFleet(Colour c) const { return const_cast<Hex*>(this)->FindFleet(c); }
	const Squadron* FindSquadron(Colour c, ShipType type) const;

	void AddShip(ShipType type, Colour colour);
	void RemoveShip(ShipType type, Colour colour);
	int GetShipCount(const Colour& c, ShipType type) const;
	bool HasShip(const Colour& c, ShipType type) const;
	bool HasShip(const Colour& c, bool bMoveableOnly = false) const;
	//bool HasEnemyShip(const Game& game, const Team* pTeam) const; // Ancients and their allies are not enemies.
	bool HasForeignShip(const Colour& c/*, bool bPlayerShipsOnly = false*/) const; // Ancients and their allies are foreign.
	//std::set<Colour> GetShipColours(bool bPlayerShipsOnly = false) const;
	bool AreAllShipsPinned(const Colour& c) const;
	bool HasPendingBattle(const Game& game) const;
	bool GetPendingBattle(Colour& defender, Colour& invader, const Game& game) const;
	bool CanAttackPopulation() const;

	int GetID() const { return m_id; }
	const MapPos& GetPos() const { return m_pos; }
	void SetPos(const MapPos& pos) { m_pos = pos; }
	int GetRotation() const { return m_nRotation; }
	const std::vector<Square>& GetSquares() const { return m_squares; }
	std::vector<Square>& GetSquares() { return m_squares; }
	const std::vector<Fleet>& GetFleets() const { return m_fleets; }
	DiscoveryType GetDiscoveryTile() const { return m_discovery; }
	EdgeSet GetWormholes() const; // Non-rotated.
	int GetVictoryPoints() const;
	bool HasArtifact() const;
	bool HasDiscovery() const;
	void RemoveDiscoveryTile();
	void SetDiscoveryTile(DiscoveryType type);
	bool CanMoveOut(const Team& team) const;
	bool CanMoveThrough(const Team& team) const;
	bool CanExploreFrom(const Team& team) const;

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

	int GetPinnage(const Team& team) const;
	
	const HexDef* m_pDef; 
	int m_id;
	MapPos m_pos;
	int m_nRotation; // [0, 5]
	std::vector<Square> m_squares;
	std::vector<Fleet> m_fleets; // In arrival order.
	DiscoveryType m_discovery;
	Colour m_colour;
	std::set<int> m_occupied;
	bool m_bOrbital, m_bMonolith;
};

typedef std::unique_ptr<Hex> HexPtr;

