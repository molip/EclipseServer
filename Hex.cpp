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

Hex::Hex() : m_id(0), m_nRotation(0), m_discovery(DiscoveryType::None), m_colour(Colour::None), m_pDef(nullptr)
{
}

Hex::Hex(int id, const MapPos& pos, int nRotation) : 
	m_id(id), m_pos(pos), m_nRotation(nRotation), m_discovery(DiscoveryType::None), m_colour(Colour::None), m_pDef(&HexDefs::Get(id))
{
	AssertThrow("Hex::Hex: Invalid rotation", nRotation >= 0 && nRotation < 6);

	for (int i = 0; i < GetDef().GetAncients(); ++i)
		AddShip(ShipType::Ancient, Colour::None);

	InitSquares();
}

Hex::Hex(const Hex& rhs) : 
	m_id(rhs.m_id), m_pos(rhs.m_pos), m_nRotation(rhs.m_nRotation), m_ships(rhs.m_ships), 
	m_discovery(rhs.m_discovery), m_colour(rhs.m_colour), m_occupied(rhs.m_occupied), m_pDef(rhs.m_pDef)
{
	InitSquares();
}

void Hex::InitSquares()
{
	AssertThrow(m_squares.empty());
	for (int i = 0; i < GetDef().GetSquareCount(); ++i)
		m_squares.push_back(Square(*this, i));
}

void Hex::SetSquareOccupied(int i, bool b) 
{
	AssertThrow("Hex::SetSquareOccupied", InRange(m_squares, i));
	if (b)
		m_occupied.insert(i);
	else
		m_occupied.erase(i);
}

bool Hex::IsSquareOccupied(int i) const 
{
	AssertThrow("Hex::IsSquareOccupied", InRange(m_squares, i));
	return m_occupied.find(i) != m_occupied.end(); 
}

bool Hex::HasWormhole(Edge e) const
{
	return GetDef().GetWormholes()[RotateEdge(e, -m_nRotation)];
}

std::vector<Square*> Hex::GetAvailableSquares(const Team& team) 
{
	AssertThrow("Hex::GetAvailableSquares", m_colour == team.GetColour());
	
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

bool Hex::HasShip(const Team* pTeam) const
{
	Colour c = pTeam ? pTeam->GetColour() : Colour::None;
	for (auto& s : m_ships)
		if (s.GetColour() == c)
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

bool Hex::HasForeignShip(const Game& game, const Team* pTeam) const
{
	for (auto& s : m_ships)
	{
		auto pShipOwner = s.GetOwner(game);
		if (pShipOwner != pTeam)
			return true;
	}
	return false;
}

void Hex::SetColour(Colour c)
{
	AssertThrowModel("Hex::SetOwner", (c == Colour::None) != (m_colour == Colour::None));
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

void Hex::RemoveDiscoveryTile()
{
	AssertThrowModel("Hex::RemoveDiscoveryTile", m_discovery != DiscoveryType::None);
	m_discovery = DiscoveryType::None;
}

void Hex::SetDiscoveryTile(DiscoveryType type)
{
	AssertThrowModel("Hex::SetDiscoveryTile", m_discovery == DiscoveryType::None);
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
}

void Hex::Load(const Serial::LoadNode& node)
{
	node.LoadType("id", m_id);
	node.LoadType("rotation", m_nRotation);
	node.LoadCntr("ships", m_ships, Serial::ClassLoader());
	node.LoadEnum("discovery", m_discovery);
	node.LoadEnum("colour", m_colour);
	node.LoadCntr("occupied", m_occupied, Serial::TypeLoader());

	m_pDef = &HexDefs::Get(m_id);
	InitSquares();
}

DEFINE_ENUM_NAMES(SquareType) { "Money", "Science", "Materials", "Any", "Orbital", "" };
