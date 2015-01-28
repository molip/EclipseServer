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
bool Square::CanOccupy(const Team& team) const { return GetRequiredTech() == TechType::None || team.HasTech(GetRequiredTech()); }
bool Square::IsOccupied() const { return m_hex.IsSquareOccupied(m_index); }
void Square::SetOccupied(bool b) { m_hex.SetSquareOccupied(m_index, b); }
SquareType Square::GetType() const { return GetDef().GetType(); }
int Square::GetX() const { return GetDef().GetX(); }
int Square::GetY() const { return GetDef().GetY(); }

//-----------------------------------------------------------------------------

const Blueprint& Ship::GetBlueprint(Colour colour, ShipType type, const Game& game)
{
	switch (type)
	{
	case ShipType::Ancient:	return Blueprint::GetAncientShip();
	case ShipType::GCDS:	return Blueprint::GetGCDS();
	}

	return game.GetTeam(colour).GetBlueprint(type);
}

//-----------------------------------------------------------------------------

Squadron::Squadron() : m_type(ShipType::None), m_colour(Colour::None), m_shipCount(0) {}
Squadron::Squadron(ShipType type) : m_type(type), m_colour(Colour::None), m_shipCount(0) {}

bool Squadron::operator==(const Squadron& rhs) const
{
	if (m_type == rhs.m_type &&
		m_shipCount == rhs.m_shipCount &&
		m_colour == rhs.m_colour)
		return true;

	return false;
}

const Blueprint& Squadron::GetBlueprint(const Game& game) const
{
	return Ship::GetBlueprint(m_colour, m_type, game);
}

void Squadron::Save(Serial::SaveNode& node) const
{
	node.SaveEnum("type", m_type);
	node.SaveType("ship_count", m_shipCount);
}

void Squadron::Load(const Serial::LoadNode& node)
{
	node.LoadEnum("type", m_type);
	node.LoadType("ship_count", m_shipCount);
}

//-----------------------------------------------------------------------------

Fleet::Fleet() : m_colour(Colour::None) {}
Fleet::Fleet(Colour colour) : m_colour(colour) {}

bool Fleet::operator==(const Fleet& rhs) const
{
	return m_squadrons == m_squadrons && m_colour == rhs.m_colour;
}

const Team* Fleet::GetOwner(const Game& game) const
{
	return m_colour == Colour::None ? nullptr : &game.GetTeam(m_colour);
}

Squadron* Fleet::FindSquadron(ShipType type)
{
	for (auto& squadron : m_squadrons)
		if (type == squadron.GetType())
			return &squadron;
	return nullptr;
}

int Fleet::GetShipCount() const
{
	int count = 0;
	for (auto& squadron : m_squadrons)
		count += squadron.GetShipCount();
	return count;
}

void Fleet::AddShip(ShipType type)
{
	auto* squadron = FindSquadron(type);
	if (!squadron)
	{
		m_squadrons.push_back(Squadron(type));
		squadron = &m_squadrons.back();
		squadron->m_colour = m_colour;
	}
	++squadron->m_shipCount;
}

void Fleet::RemoveShip(ShipType type)
{
	for (auto it = m_squadrons.begin(); it != m_squadrons.end(); ++it)
		if (it->GetType() == type)
		{
			VERIFY_MODEL(it->m_shipCount > 0);
			
			if (--it->m_shipCount == 0)
				m_squadrons.erase(it);
			return;
		}

	VERIFY_MODEL(false);
}

void Fleet::Save(Serial::SaveNode& node) const
{
	node.SaveCntr("squadrons", m_squadrons, Serial::ClassSaver());
	node.SaveEnum("colour", m_colour);
}

void Fleet::Load(const Serial::LoadNode& node)
{
	node.LoadCntr("squadrons", m_squadrons, Serial::ClassLoader());
	node.LoadEnum("colour", m_colour);

	for (auto& s : m_squadrons)
		s.m_colour = m_colour;
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
	VERIFY_MODEL_MSG("Invalid rotation", nRotation >= 0 && nRotation < 6);

	for (int i = 0; i < GetDef().GetAncients(); ++i)
		AddShip(ShipType::Ancient, Colour::None);

	InitSquares();
}

Hex::Hex(const Hex& rhs) : 
	m_id(rhs.m_id), m_pos(rhs.m_pos), m_nRotation(rhs.m_nRotation), m_fleets(rhs.m_fleets),
	m_discovery(rhs.m_discovery), m_colour(rhs.m_colour), m_occupied(rhs.m_occupied), m_pDef(rhs.m_pDef),
	m_bOrbital(rhs.m_bOrbital), m_bMonolith(rhs.m_bMonolith)
{
	InitSquares();
}

bool Hex::operator==(const Hex& rhs) const
{
	if (m_id == rhs.m_id &&
		m_pos == rhs.m_pos &&
		m_nRotation == rhs.m_nRotation &&
		m_squares == rhs.m_squares &&
		m_fleets == rhs.m_fleets &&
		m_discovery == rhs.m_discovery &&
		m_colour == rhs.m_colour &&
		m_occupied == rhs.m_occupied &&
		m_bOrbital == rhs.m_bOrbital &&
		m_bMonolith == rhs.m_bMonolith)
		return true;

	return false;
}

void Hex::InitSquares()
{
	VERIFY_MODEL(m_squares.empty());
	for (int i = 0; i < GetDef().GetSquareCount(); ++i)
		m_squares.push_back(Square(*this, i));
}

void Hex::SetSquareOccupied(int i, bool b) 
{
	VERIFY_MODEL(InRange(m_squares, i));
	if (b)
		m_occupied.insert(i);
	else
		m_occupied.erase(i);
}

bool Hex::IsSquareOccupied(int i) const 
{
	VERIFY_MODEL(InRange(m_squares, i));
	return m_occupied.find(i) != m_occupied.end(); 
}

bool Hex::HasWormhole(Edge e) const
{
	return GetDef().GetWormholes()[RotateEdge(e, -m_nRotation)];
}

std::vector<Square*> Hex::GetAvailableSquares(const Team& team) 
{
	VERIFY_MODEL(m_colour == team.GetColour());
	
	std::vector<Square*> squares;

	for (Square& s : m_squares)
		if (!s.IsOccupied() && s.CanOccupy(team))
			squares.push_back(&s);
	return squares;
}

SquareCounts Hex::GetAvailableSquareCounts(const Team& team) const
{
	SquareCounts counts;
	for (auto& s : m_squares)
		if (!s.IsOccupied() && s.CanOccupy(team))
			++counts[s.GetType()];
	return counts;
}

SquareCounts Hex::GetOccupiedSquareCounts() const
{
	SquareCounts counts;
	for (auto& s : m_squares)
		if (s.IsOccupied())
			++counts[s.GetType()];
	return counts;
}

bool Hex::HasAnyOccupiedSquares() const
{
	for (auto& s : m_squares)
		if (s.IsOccupied())
			return true;;
	return false;
}
Fleet* Hex::FindFleet(Colour c) 
{
	for (auto& f : m_fleets)
		if (f.GetColour() == c)
			return &f;
	return nullptr;
}

const Squadron* Hex::FindSquadron(Colour c, ShipType type) const
{
	auto fleet = FindFleet(c);
	return fleet ? fleet->FindSquadron(type) : nullptr;
}

void Hex::AddShip(ShipType type, Colour colour)
{
	auto* fleet = FindFleet(colour);
	if (!fleet)
	{
		m_fleets.push_back(Fleet(colour));
		fleet = &m_fleets.back();
	}
	fleet->AddShip(type);
}

void Hex::RemoveShip(ShipType type, Colour colour)
{
	for (auto fleetIt = m_fleets.begin(); fleetIt != m_fleets.end(); ++fleetIt)
		if (fleetIt->GetColour() == colour)
		{
			fleetIt->RemoveShip(type);
			if (fleetIt->GetSquadrons().empty())
				m_fleets.erase(fleetIt);
			return;
		}

	VERIFY_MODEL(false);
}

bool Hex::HasShip(const Colour& c, ShipType type) const
{
	return GetShipCount(c, type) > 0;
}

int Hex::GetShipCount(const Colour& c, ShipType type) const
{
	auto squad = FindSquadron(c, type);
	return squad ? squad->GetShipCount() : 0;
}

bool Hex::HasShip(const Colour& c, bool bMoveableOnly) const
{
	if (auto fleet = FindFleet(c))
		if (bMoveableOnly)
			return fleet->GetSquadrons().size() > 1 || !fleet->FindSquadron(ShipType::Starbase);
		else
			return true;
	return false;
}

bool Hex::HasPendingBattle(const Game& game) const
{
	if (IsOwned() && HasForeignShip(m_colour))
		return true;

	if (m_fleets.size() < 2)
		return false;

	if (m_fleets.size() == 2)
		if (HasShip(Colour::None, ShipType::Ancient))
			if (Race(game.GetTeam(m_fleets.back().GetColour()).GetRace()).IsAncientsAlly())
				return false;
	return true;
}

bool Hex::GetPendingBattle(Colour& defender, Colour& invader, const Game& game) const
{
	if (!HasPendingBattle(game))
		return false;

	if (m_fleets.size() == 1)
	{
		invader = m_fleets.front().GetColour();
		defender = m_colour;
		VERIFY(invader != defender);
	}
	else
	{
		std::list<Colour> colours;
		for (auto& f : m_fleets)
			if (f.GetColour() == m_colour)
				colours.push_front(f.GetColour());
			else
				colours.push_back(f.GetColour());

		invader = colours.back();
		colours.pop_back();
		defender = colours.back();
	}
	return true;
}

// Doesn't check if any squadron has cannons. 
bool Hex::CanAttackPopulation() const
{
	return !m_occupied.empty() && m_fleets.size() == 1 && m_fleets.front().GetColour() != m_colour;
}

//bool Hex::HasEnemyShip(const Game& game, const Team* pTeam) const
//{
//	Colour c = pTeam ? pTeam->GetColour() : Colour::None;
//	for (auto& s : m_ships)
//	{
//		auto pShipOwner = s.GetOwner(game);
//		if (s.GetColour() != c && !Team::IsAncientAlliance(pTeam, pShipOwner))
//			return true;
//	}
//	return false;
//}

bool Hex::HasForeignShip(const Colour& c/*, bool bPlayerShipsOnly*/) const
{
	return !m_fleets.empty() && !(m_fleets.size() == 1 && HasShip(c));
}

//std::set<Colour> Hex::GetShipColours(bool bPlayerShipsOnly) const
//{
//	std::set<Colour> set;
//	for (auto& s : m_ships)
//		if (!bPlayerShipsOnly || s.GetColour() != Colour::None)
//			set.insert(s.GetColour());
//	return set;
//}

int Hex::GetPinnage(const Team& team) const
{
	int nPinnage = 0;
	for (auto& fleet : m_fleets)
	{
		if (fleet.FindSquadron(ShipType::GCDS))
			return 1000;
		if (!(Race(team.GetRace()).IsAncientsAlly() && fleet.GetColour() == Colour::None))
			nPinnage += (fleet.GetColour() == team.GetColour() ? -1 : 1) * fleet.GetShipCount();
	}
	return nPinnage;
}

// NB. Ignores the immoveability of space stations. 
bool Hex::CanMoveOut(const Team& team) const
{
	return GetPinnage(team) < 0;
}

bool Hex::CanMoveThrough(const Team& team) const
{
	return GetPinnage(team) <= 0;
}

bool Hex::CanExploreFrom(const Team& team) const
{
	// "...next to a hex where you have a Ship or an Influence Disc"
	// "If you Explore from a hex with only a Ship, it must not be pinned"
		return IsOwnedBy(team) || (HasShip(team.GetColour()) && CanMoveOut(team));
}

void Hex::SetColour(Colour c)
{
	VERIFY_MODEL((c == Colour::None) != (m_colour == Colour::None));
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
	VERIFY_MODEL(m_discovery != DiscoveryType::None);
	m_discovery = DiscoveryType::None;
}

void Hex::SetDiscoveryTile(DiscoveryType type)
{
	VERIFY_MODEL(m_discovery == DiscoveryType::None);
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
	node.SaveCntr("fleets", m_fleets, Serial::ClassSaver());
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
	node.LoadCntr("fleets", m_fleets, Serial::ClassLoader());
	node.LoadEnum("discovery", m_discovery);
	node.LoadEnum("colour", m_colour);
	node.LoadCntr("occupied", m_occupied, Serial::TypeLoader());
	node.LoadType("has_orbital", m_bOrbital);
	node.LoadType("has_monolith", m_bMonolith);

	m_pDef = &HexDefs::Get(m_id);
	InitSquares();
}

DEFINE_ENUM_NAMES(SquareType) { "Money", "Science", "Materials", "Any", "Orbital", "" };
