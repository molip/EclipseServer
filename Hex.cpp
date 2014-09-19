#include "stdafx.h"
#include "Hex.h"
#include "App.h"
#include "Discovery.h"
#include "Bag.h"
#include "Ship.h"
#include "Game.h"
#include "Technology.h"
#include "HexDefs.h"
#include "EnumTraits.h"
#include "Serial.h"

Square::Square(Hex& hex, int index) : m_hex(hex), m_index(index)
{
}

const SquareDef& Square::GetDef() const
{
	return m_hex.GetDef().GetSquare(m_index);
}

TechType Square::GetRequiredTech() const { return GetDef().GetRequiredTech(); }
bool Square::IsOccupied() const { return m_hex.IsSquareOccupied(m_index); }
void Square::SetOccupied(bool b) { m_hex.SetSquareOccupied(m_index, b); }
SquareType Square::GetType() const { return GetDef().GetType(); }
int Square::GetX() const { return GetDef().GetX(); }
int Square::GetY() const { return GetDef().GetY(); }

//-----------------------------------------------------------------------------

Ship::Ship() : m_type(ShipType::None), m_colour(Colour::None) 
{
}

const Team* Ship::GetOwner(const Game& game) const
{
	return m_colour == Colour::None ? nullptr : &game.GetTeam(m_colour);
}

const Blueprint& Ship::GetBlueprint(const Game& game) const
{
	switch (m_type)
	{
	case ShipType::Ancient:	return Blueprint::GetAncientShip();
	case ShipType::GCDS:	return Blueprint::GetGCDS();
	}

	return game.GetTeam(m_colour).GetBlueprint(m_type);
}

void Ship::Save(Serial::SaveNode& node) const
{
	node.SaveEnum("type", m_type);
	node.SaveEnum("colour", m_colour);
}

void Ship::Load(const Serial::LoadNode& node)
{
	node.LoadEnum("type", m_type);
	node.LoadEnum("colour", m_colour);
}

//-----------------------------------------------------------------------------

Hex::Hex() : 
	m_id(0), m_nRotation(0), m_discovery(DiscoveryType::None), m_colour(Colour::None), m_pDef(nullptr), 
	m_bOrbital(false), m_bMonolith(false)
{
}

Hex::Hex(int id, const MapPos& pos, int nRotation) : 
	m_id(id), m_pos(pos), m_nRotation(nRotation), m_discovery(DiscoveryType::None), m_colour(Colour::None), m_pDef(&HexDefs::Get(id)),
	m_bOrbital(false), m_bMonolith(false)
{
	VerifyModel("Hex::Hex: Invalid rotation", nRotation >= 0 && nRotation < 6);

	for (int i = 0; i < GetDef().GetAncients(); ++i)
		AddShip(ShipType::Ancient, Colour::None);

	InitSquares();
}

Hex::Hex(const Hex& rhs) : 
	m_id(rhs.m_id), m_pos(rhs.m_pos), m_nRotation(rhs.m_nRotation), m_ships(rhs.m_ships), 
	m_discovery(rhs.m_discovery), m_colour(rhs.m_colour), m_occupied(rhs.m_occupied), m_pDef(rhs.m_pDef),
	m_bOrbital(rhs.m_bOrbital), m_bMonolith(rhs.m_bMonolith)
{
	InitSquares();
}

void Hex::InitSquares()
{
	VerifyModel("Hex::InitSquares", m_squares.empty());
	for (int i = 0; i < GetDef().GetSquareCount(); ++i)
		m_squares.push_back(Square(*this, i));
}

void Hex::SetSquareOccupied(int i, bool b) 
{
	VerifyModel("Hex::SetSquareOccupied", InRange(m_squares, i));
	if (b)
		m_occupied.insert(i);
	else
		m_occupied.erase(i);
}

bool Hex::IsSquareOccupied(int i) const 
{
	VerifyModel("Hex::IsSquareOccupied", InRange(m_squares, i));
	return m_occupied.find(i) != m_occupied.end(); 
}

bool Hex::HasWormhole(Edge e) const
{
	return GetDef().GetWormholes()[RotateEdge(e, -m_nRotation)];
}

std::vector<Square*> Hex::GetAvailableSquares(const Team& team) 
{
	VerifyModel("Hex::GetAvailableSquares", m_colour == team.GetColour());
	
	std::vector<Square*> squares;

	for (Square& s : m_squares)
		if (!s.IsOccupied() && (s.GetRequiredTech() == TechType::None || team.HasTech(s.GetRequiredTech())))
			squares.push_back(&s);
	return squares;
}

void Hex::AddShip(ShipType type, Colour owner)
{
	m_ships.push_back(Ship(type, owner));
}

void Hex::RemoveShip(ShipType type, Colour owner)
{
	for (auto s = m_ships.begin(); s != m_ships.end(); ++s)
		if (s->GetType() == type && s->GetColour() == owner)
		{
			m_ships.erase(s);
			return;
		}
	VerifyModel("Hex::RemoveShip");
}

bool Hex::HasShip(const Colour& c, ShipType ship) const
{
	for (auto& s : m_ships)
		if (s.GetColour() == c && s.GetType() == ship)
			return true;
	return false;
}

int Hex::GetShipCount(const Colour& c, ShipType ship) const
{
	int count = 0;
	for (auto& s : m_ships)
		count += s.GetColour() == c && s.GetType() == ship;
	return count;
}

bool Hex::HasShip(const Colour& c, bool bMoveableOnly) const
{
	for (auto& s : m_ships)
		if (s.GetColour() == c && (!bMoveableOnly || s.GetType() != ShipType::Starbase))
			return true;
	return false;
}

bool Hex::HasEnemyShip(const Game& game, const Team* pTeam) const
{
	Colour c = pTeam ? pTeam->GetColour() : Colour::None;
	for (auto& s : m_ships)
	{
		auto pShipOwner = s.GetOwner(game);
		if (s.GetColour() != c && !Team::IsAncientAlliance(pTeam, pShipOwner))
			return true;
	}
	return false;
}

bool Hex::HasForeignShip(const Colour& c, bool bPlayerShipsOnly) const
{
	for (auto& s : m_ships)
		if (s.GetColour() != c)
			if (!bPlayerShipsOnly || s.GetColour() != Colour::None)
				return true;
	return false;
}

std::set<Colour> Hex::GetShipColours(bool bPlayerShipsOnly) const
{
	std::set<Colour> set;
	for (auto& s : m_ships)
		if (!bPlayerShipsOnly || s.GetColour() != Colour::None)
			set.insert(s.GetColour());
	return set;
}

int Hex::GetPinnage(const Team& team) const
{
	int nPinnage = 0;
	for (auto& s : m_ships)
	{
		if (s.GetType() == ShipType::GCDS)
			return 1000;
		if (!(Race(team.GetRace()).IsAncientsAlly() && s.GetType() == ShipType::Ancient))
			nPinnage += s.GetColour() == team.GetColour() ? -1 : 1;
	}
	return nPinnage;
}

bool Hex::CanMoveOut(const Team& team) const
{
	return GetPinnage(team) < 0;
}

bool Hex::CanMoveThrough(const Team& team) const
{
	return GetPinnage(team) <= 0;
}

void Hex::SetColour(Colour c)
{
	VerifyModel("Hex::SetOwner", (c == Colour::None) != (m_colour == Colour::None));
	m_colour = c;
}

bool Hex::IsOwned() const
{
	return m_colour != Colour::None;
}

bool Hex::IsOwnedBy(const Team& team) const
{
	return m_colour == team.GetColour();
}

bool Hex::HasNeighbour(const Map& map, bool bWormholeGen) const
{
	return map.HasNeighbour(m_pos, bWormholeGen);
}

void Hex::RemoveDiscoveryTile()
{
	VerifyModel("Hex::RemoveDiscoveryTile", m_discovery != DiscoveryType::None);
	m_discovery = DiscoveryType::None;
}

void Hex::SetDiscoveryTile(DiscoveryType type)
{
	VerifyModel("Hex::SetDiscoveryTile", m_discovery == DiscoveryType::None);
	m_discovery = type;
}

Square* Hex::FindSquare(SquareType type, bool bOccupied)
{
	for (auto& s : m_squares)
		if (s.GetType() == type && s.IsOccupied() == bOccupied)
			return &s;
	return nullptr;
}

const EdgeSet& Hex::GetWormholes() const { return GetDef().GetWormholes(); } 
int Hex::GetVictoryPoints() const { return GetDef().GetVictoryPoints(); }
bool Hex::HasArtifact() const { return GetDef().HasArtifact(); }
bool Hex::HasDiscovery() const { return GetDef().HasDiscovery(); }

void Hex::Save(Serial::SaveNode& node) const
{
	node.SaveType("id", m_id);
	node.SaveType("rotation", m_nRotation);
	node.SaveCntr("ships", m_ships, Serial::ClassSaver());
	node.SaveEnum("discovery", m_discovery);
	node.SaveEnum("colour", m_colour);
	node.SaveCntr("occupied", m_occupied, Serial::TypeSaver());
	node.SaveType("has_orbital", m_bOrbital);
	node.SaveType("has_monolith", m_bMonolith);
}

void Hex::Load(const Serial::LoadNode& node)
{
	node.LoadType("id", m_id);
	node.LoadType("rotation", m_nRotation);
	node.LoadCntr("ships", m_ships, Serial::ClassLoader());
	node.LoadEnum("discovery", m_discovery);
	node.LoadEnum("colour", m_colour);
	node.LoadCntr("occupied", m_occupied, Serial::TypeLoader());
	node.LoadType("has_orbital", m_bOrbital);
	node.LoadType("has_monolith", m_bMonolith);

	m_pDef = &HexDefs::Get(m_id);
	InitSquares();
}

DEFINE_ENUM_NAMES(SquareType) { "Money", "Science", "Materials", "Any", "Orbital", "" };
