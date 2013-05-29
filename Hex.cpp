#include "stdafx.h"
#include "Hex.h"
#include "App.h"
#include "Discovery.h"
#include "Bag.h"
#include "Ship.h"
#include "Game.h"
#include "Technology.h"
#include "EnumTraits.h"

Edge ReverseEdge(Edge e)
{
	return RotateEdge(e, 3);
}

Edge RotateEdge(Edge e, int n)
{
	return (Edge)Mod(int(e) + n, 6);
}

//-----------------------------------------------------------------------------

EdgeSet::EdgeSet(std::string s)
{
	std::reverse(s.begin(), s.end());
	__super::operator =(std::bitset<6>(s));
}

bool EdgeSet::operator[](Edge e) const
{
	return __super::operator[]((int)e);
}

EdgeSet::reference EdgeSet::operator[](Edge e)
{
	return __super::operator[]((int)e);
}

std::vector<Edge> EdgeSet::GetEdges() const
{
	std::vector<Edge> edges;
	for (int i = 0; i < 6; ++i)
		if (__super::operator[](i))
			edges.push_back((Edge)i);
	return edges;
}

//-----------------------------------------------------------------------------

Square::Square(int x, int y, SquareType type, bool bAdvanced) : m_x(x), m_y(y), m_type(type), m_bAdvanced(bAdvanced), m_bOccupied(false)
{
}

TechType Square::GetRequiredTech() const
{
	if (m_bAdvanced)
		switch (m_type)
		{
		case SquareType::Materials: return TechType::AdvMining;
		case SquareType::Money: return TechType::AdvEconomy;
		case SquareType::Science: return TechType::AdvLabs;
		}
	return TechType::None;
}

//-----------------------------------------------------------------------------
const Team* Ship::GetOwner(const Game& game) const
{
	return m_colour == Colour::None ? nullptr : &game.GetTeam(m_colour);
}

//-----------------------------------------------------------------------------

Hex::Hex(Game* pGame, int id, const MapPos& pos, int nRotation) : 
	m_id(id), m_pos(pos), m_nRotation(nRotation), m_discovery(DiscoveryType::None), 
	m_nVictory(0), m_bArtifact(false), m_colour(Colour::None)
{
	AssertThrow("Hex::Hex: Invalid rotation", nRotation >= 0 && nRotation < 6);
	Init(pGame);
}

bool Hex::HasWormhole(Edge e) const
{
	return m_wormholes[RotateEdge(e, -m_nRotation)];
}

std::vector<Square*> Hex::GetAvailableSquares(const Team& team) 
{
	AssertThrow("Hex::GetAvailableSquares", m_colour == team.GetColour());
	
	std::vector<Square*> squares;

	for (Square& s : m_squares)
		if (!s.IsOccupied() && team.HasTech(s.GetRequiredTech()))
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

Square* Hex::FindSquare(SquareType type, bool bOccupied)
{
	for (auto& s : m_squares)
		if (s.GetType() == type && s.IsOccupied() == bOccupied)
			return &s;
	return nullptr;
}

void Hex::Init(Game* pGame)
{
	auto AddSquare = [&] (int x, int y, SquareType type, bool bAdvanced) { m_squares.push_back(Square(x, y, type, bAdvanced)); };
	auto SetWormholes = [&] (std::string s) { m_wormholes = EdgeSet(s); };

	bool bDiscovery = false;
	int nAncients = 0;

	switch(m_id)
	{
	case 001:
		AddSquare(299,  92, SquareType::Science, false);
		AddSquare(250,  92, SquareType::Science, true);
		AddSquare( 95, 235, SquareType::Materials, false);
		AddSquare( 95, 176, SquareType::Materials, true);
		AddSquare(218, 328, SquareType::Any, false);
		AddSquare(270, 328, SquareType::Any, false);
		SetWormholes("111111");
		m_nVictory = 4, m_bArtifact = true;
		AddShip(ShipType::GCDS, Colour::None);
		break;
	case 101:
		AddSquare(310, 113, SquareType::Money, false);
		AddSquare(134,  87, SquareType::Materials, false);
		AddSquare(182,  87, SquareType::Materials, true);
		SetWormholes("011111");
		m_nVictory = 2, nAncients = 1;
		break;
	case 102:
		AddSquare(138, 266, SquareType::Science, false);
		AddSquare(205, 266, SquareType::Science, true);
		SetWormholes("101101");
		m_nVictory = 3, m_bArtifact = true;
		break;
	case 103:
		AddSquare(195, 271, SquareType::Money, true);
		AddSquare(286, 293, SquareType::Any, false);
		SetWormholes("111011");
		m_nVictory = 2;
		break;
	case 104:
		AddSquare(322, 104, SquareType::Money, false);
		AddSquare(275, 104, SquareType::Money, true);
		AddSquare(124, 287, SquareType::Science, false);
		AddSquare(124, 238, SquareType::Science, true);
		SetWormholes("101101");
		m_nVictory = 2, nAncients = 2;
		break;
	case 105:
		AddSquare(281,  83, SquareType::Money, false);
		AddSquare( 84, 220, SquareType::Science, false);
		AddSquare(296, 288, SquareType::Materials, true);
		SetWormholes("111101");
		m_nVictory = 2, m_bArtifact = true, nAncients = 1;
		break;
	case 106:
		AddSquare(167, 144, SquareType::Science, false);
		AddSquare(267, 125, SquareType::Materials, false);
		SetWormholes("111100");
		m_nVictory = 2;
		break;
	case 107:
		AddSquare(149, 252, SquareType::Money, false);
		AddSquare(243, 123, SquareType::Science, true);
		AddSquare(253, 269, SquareType::Materials, true);
		SetWormholes("111101");
		m_nVictory = 2;
		break;
	case 108:
		AddSquare(135, 143, SquareType::Money, true);
		AddSquare(247,  84, SquareType::Science, false);
		AddSquare(273, 316, SquareType::Any, false);
		SetWormholes("110110");
		m_nVictory = 2, nAncients = 2;
		break;
	case 201:
		AddSquare(145, 241, SquareType::Money, false);
		AddSquare(204, 123, SquareType::Materials, false);
		SetWormholes("010101");
		m_nVictory = 1;
		break;
	case 202:
		AddSquare(186, 311, SquareType::Science, false);
		AddSquare(186, 260, SquareType::Science, true);
		SetWormholes("010101");
		m_nVictory = 1;
		break;
	case 203:
		AddSquare(158, 306, SquareType::Money, false);
		AddSquare(285, 298, SquareType::Science, false);
		AddSquare(182,  87, SquareType::Materials, false);
		SetWormholes("110101");
		m_nVictory = 1, nAncients = 2;
		break;
	case 204:
		AddSquare(166, 76, SquareType::Money, true);
		AddSquare(174, 300, SquareType::Materials, true);
		AddSquare(294, 116, SquareType::Any, false);
		SetWormholes("110101");
		m_nVictory = 2, nAncients = 1, m_bArtifact  = true;
		break;
	case 205:
		AddSquare(171, 116, SquareType::Money, false);
		AddSquare(223, 116, SquareType::Money, true);
		AddSquare(253, 271, SquareType::Science, true);
		SetWormholes("001110");
		m_nVictory = 1;
		break;
	case 206:
		AddSquare(167, 116, SquareType::Materials, false);
		SetWormholes("011101");
		m_nVictory = 1, bDiscovery = true;
		break;
	case 207:
		SetWormholes("110100");
		m_nVictory = 1, bDiscovery = true;
		break;
	case 208:
		SetWormholes("101101");
		m_nVictory = 1, bDiscovery = true;
		break;
	case 209:
		AddSquare(270, 108, SquareType::Money, true);
		AddSquare(159, 191, SquareType::Science, false);
		SetWormholes("110101");
		m_nVictory = 1;
		break;
	case 210:
		AddSquare(243,  99, SquareType::Money, false);
		AddSquare(315, 253, SquareType::Materials, false);
		SetWormholes("100101");
		m_nVictory = 1;
		break;
	case 211:
		AddSquare(283, 100, SquareType::Money, false);
		AddSquare(280, 307, SquareType::Materials, true);
		AddSquare(103, 261, SquareType::Any, false);
		SetWormholes("111100");
		m_nVictory = 2, nAncients = 1, m_bArtifact  = true;
		break;

	case 221:
		AddSquare(191, 102, SquareType::Money, false);
		AddSquare(247, 102, SquareType::Money, true);
		AddSquare(184, 294, SquareType::Science, false);
		AddSquare(133, 294, SquareType::Science, true);
		AddSquare(320, 291, SquareType::Materials, false);
		break;
	case 222:
		AddSquare(198, 106, SquareType::Money, false);
		AddSquare(250, 106, SquareType::Money, true);
		AddSquare(268, 274, SquareType::Science, false);
		AddSquare(318, 274, SquareType::Science, true);
		break;
	case 223:
		AddSquare(212, 105, SquareType::Money, false);
		AddSquare(271, 105, SquareType::Money, true);
		AddSquare(156, 192, SquareType::Science, false);
		AddSquare( 98, 192, SquareType::Science, true);
		AddSquare(299, 271, SquareType::Materials, false);
		break;
	case 224:
		AddSquare(212, 112, SquareType::Money, false);
		AddSquare(156, 192, SquareType::Science, false);
		AddSquare( 98, 192, SquareType::Science, true);
		AddSquare(258, 279, SquareType::Materials, true);
		break;
	case 225:
		AddSquare(169, 309, SquareType::Money, false);
		AddSquare(169, 256, SquareType::Money, true);
		AddSquare(285, 262, SquareType::Science, false);
		AddSquare(336, 262, SquareType::Science, true);
		AddSquare(169, 124, SquareType::Materials, false);
		break;
	case 226:
		AddSquare(191, 107, SquareType::Money, false);
		AddSquare(158, 294, SquareType::Science, false);
		AddSquare(342, 262, SquareType::Materials, false);
		break;
	case 227:
		AddSquare(276, 285, SquareType::Money, false);
		AddSquare(333, 285, SquareType::Money, true);
		AddSquare(224, 118, SquareType::Science, false);
		AddSquare(224,  69, SquareType::Science, true);
		AddSquare(126, 250, SquareType::Materials, false);
		break;
	case 228:
		AddSquare(183, 110, SquareType::Money, false);
		AddSquare(169, 263, SquareType::Science, false);
		AddSquare(291, 282, SquareType::Materials, true);
		break;
	case 229:
		AddSquare(132, 274, SquareType::Money, false);
		AddSquare(188, 274, SquareType::Money, true);
		AddSquare(278, 273, SquareType::Science, false);
		AddSquare(330, 273, SquareType::Science, true);
		AddSquare(289,  79, SquareType::Materials, false);
		break;
	case 230:
		AddSquare(180, 107, SquareType::Money, false);
		AddSquare(235, 107, SquareType::Money, true);
		AddSquare(146, 285, SquareType::Science, false);
		AddSquare(341, 262, SquareType::Materials, true);
		break;
	case 231:
		AddSquare(277, 265, SquareType::Money, false);
		AddSquare(335, 265, SquareType::Money, true);
		AddSquare(252, 112, SquareType::Science, false);
		AddSquare(194, 112, SquareType::Science, true);
		AddSquare(155, 279, SquareType::Materials, false);
		break;
	case 232:
		AddSquare(230, 106, SquareType::Money, true);
		AddSquare(131, 210, SquareType::Science, false);
		AddSquare(310, 276, SquareType::Materials, false);
		AddSquare(251, 276, SquareType::Materials, true);
		break;

	case 301:
		AddSquare(302, 102, SquareType::Money, false);
		AddSquare(112, 182, SquareType::Science, false);
		AddSquare(294, 218, SquareType::Materials, true);
		SetWormholes("101100");
		m_nVictory = 2, nAncients = 2, m_bArtifact  = true;
		break;
	case 302:
		AddSquare(277,  94, SquareType::Money, true);
		AddSquare(305, 304, SquareType::Materials, false);
		SetWormholes("100110");
		m_nVictory = 2, nAncients = 1, m_bArtifact  = true;
		break;
	case 303:
		AddSquare(215, 86, SquareType::Any, false);
		SetWormholes("000101");
		m_nVictory = 2, nAncients = 1, m_bArtifact  = true;
		break;
	case 304:
		AddSquare(165, 126, SquareType::Money, true);
		AddSquare(216, 281, SquareType::Materials, false);
		SetWormholes("100100");
		m_nVictory = 2;
		break;
	case 305:
		AddSquare(133, 110, SquareType::Science, false);
		AddSquare(260, 299, SquareType::Materials, false);
		SetWormholes("110100");
		m_nVictory = 1, nAncients = 1;
		break;
	case 306:
		AddSquare(260, 113, SquareType::Money, false);
		AddSquare(269, 284, SquareType::Materials, false);
		SetWormholes("010100");
		m_nVictory = 1;
		break;
	case 307:
		AddSquare(219,  81, SquareType::Money, false);
		AddSquare(146, 251, SquareType::Science, true);
		SetWormholes("101100");
		m_nVictory = 1;
		break;
	case 308:
		AddSquare(283, 118, SquareType::Science, false);
		AddSquare(155, 197, SquareType::Materials, true);
		SetWormholes("001101");
		m_nVictory = 1;
		break;
	case 309:
		AddSquare(254, 126, SquareType::Money, false);
		AddSquare(145, 207, SquareType::Science, true);
		SetWormholes("100101");
		m_nVictory = 1;
		break;
	case 310:
		AddSquare(264,  76, SquareType::Science, false);
		AddSquare(156, 248, SquareType::Materials, false);
		SetWormholes("100100");
		m_nVictory = 1;
		break;
	case 311:
		AddSquare(288, 274, SquareType::Materials, false);
		SetWormholes("101100");
		m_nVictory = 1, bDiscovery = true;
		break;
	case 312:
		AddSquare(251, 97, SquareType::Materials, false);
		SetWormholes("110100");
		m_nVictory = 1, bDiscovery = true;
		break;
	case 313:
		AddSquare(263, 101, SquareType::Any, false);
		SetWormholes("100100");
		m_nVictory = 1, bDiscovery = true;
		break;
	case 314:
		AddSquare(101, 244, SquareType::Any, false);
		SetWormholes("001110");
		m_nVictory = 1, bDiscovery = true;
		break;
	case 315:
		SetWormholes("100101");
		m_nVictory = 1, bDiscovery = true;
		break;
	case 316:
		SetWormholes("110100");
		m_nVictory = 1, bDiscovery = true;
		break;
	case 317:
		AddSquare(167, 265, SquareType::Money, false);
		AddSquare(167, 316, SquareType::Money, true);
		SetWormholes("000110");
		m_nVictory = 1;
		break;
	case 318:
		AddSquare(113, 236, SquareType::Materials, true);
		AddSquare(221, 106, SquareType::Any, false);
		SetWormholes("001100");
		m_nVictory = 1;
		break;
	}

	if (m_id >= 221 && m_id <= 232) // Starting hex.
	{
		SetWormholes("110110");
		m_nVictory = 3, m_bArtifact = true;
	}

	for (int i = 0; i < nAncients; ++i)
		AddShip(ShipType::Ancient, Colour::None);

	bDiscovery |= nAncients > 0;

	if (bDiscovery && pGame)
		m_discovery = pGame->GetDiscoveryBag().TakeTile();
}

DEFINE_ENUM_NAMES(SquareType) { "Money", "Science", "Materials", "Any", "Orbital", "" };
