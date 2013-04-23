#include "stdafx.h"
#include "Hex.h"
#include "App.h"
#include "Discovery.h"
#include "Bag.h"
#include "Ship.h"
#include "Game.h"
#include "Technology.h"

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

Square::Square(int x, int y, Resource type, bool bAdvanced) : m_x(x), m_y(y), m_type(type), m_bAdvanced(bAdvanced), m_pOwner(nullptr)
{
}

TechType Square::GetRequiredTech() const
{
	if (m_bAdvanced)
		switch (m_type)
		{
		case Resource::Materials: return TechType::AdvMining;
		case Resource::Money: return TechType::AdvEconomy;
		case Resource::Science: return TechType::AdvLabs;
		}
	return TechType::None;
}

void Square::SetOwner(Team* pOwner)
{
	AssertThrowModel("Square::SetOwner", !m_pOwner);
	m_pOwner = pOwner;
}

//-----------------------------------------------------------------------------

Hex::Hex(Game* pGame, int id, int nRotation) : 
	m_id(id), m_nRotation(nRotation), m_discovery(DiscoveryType::None), 
	m_nVictory(0), m_bArtifact(false), m_pOwner(nullptr)
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
	std::vector<Square*> squares;

	for (Square& s : m_squares)
		if (!s.GetOwner() && team.HasTech(s.GetRequiredTech()))
			squares.push_back(&s);
	return squares;
}

void Hex::AddShip(ShipType type, Team* pOwner)
{
	m_ships.push_back(Ship(type, pOwner));
}

bool Hex::HasShip(const Team* pOwner) const
{
	for (auto& s : m_ships)
		if (s.GetOwner() == pOwner)
			return true;
	return false;
}

bool Hex::HasEnemyShip(const Team* pTeam) const
{
	const bool bAncientsAlly = pTeam && Race(pTeam->GetRace()).IsAncientsAlly();

	for (auto& s : m_ships)
		if (s.GetOwner() != pTeam && !(bAncientsAlly && s.GetOwner() == nullptr))
			return true;
	return false;
}

void Hex::SetOwner(Team* pOwner)
{
	AssertThrowModel("Hex::SetOwner", (pOwner == nullptr) != (m_pOwner == nullptr));
	m_pOwner = pOwner;
}

void Hex::Init(Game* pGame)
{
	auto AddSquare = [&] (int x, int y, Resource type, bool bAdvanced) { m_squares.push_back(Square(x, y, type, bAdvanced)); };
	auto SetWormholes = [&] (std::string s) { m_wormholes = EdgeSet(s); };

	bool bDiscovery = false;
	int nAncients = 0;

	switch(m_id)
	{
	case 001:
		AddSquare(299,  92, Resource::Science, false);
		AddSquare(250,  92, Resource::Science, true);
		AddSquare( 95, 235, Resource::Materials, false);
		AddSquare( 95, 176, Resource::Materials, true);
		AddSquare(218, 328, Resource::Any, false);
		AddSquare(270, 328, Resource::Any, false);
		SetWormholes("111111");
		m_nVictory = 4, m_bArtifact = true;
		AddShip(ShipType::GCDS, nullptr);
		break;
	case 101:
		AddSquare(310, 113, Resource::Money, false);
		AddSquare(134,  87, Resource::Materials, false);
		AddSquare(182,  87, Resource::Materials, true);
		SetWormholes("011111");
		m_nVictory = 2, nAncients = 1;
		break;
	case 102:
		AddSquare(138, 266, Resource::Science, false);
		AddSquare(205, 266, Resource::Science, true);
		SetWormholes("101101");
		m_nVictory = 3, m_bArtifact = true;
		break;
	case 103:
		AddSquare(195, 271, Resource::Money, true);
		AddSquare(286, 293, Resource::Any, false);
		SetWormholes("111011");
		m_nVictory = 2;
		break;
	case 104:
		AddSquare(322, 104, Resource::Money, false);
		AddSquare(275, 104, Resource::Money, true);
		AddSquare(124, 287, Resource::Science, false);
		AddSquare(124, 238, Resource::Science, true);
		SetWormholes("101101");
		m_nVictory = 2, nAncients = 2;
		break;
	case 105:
		AddSquare(281,  83, Resource::Money, false);
		AddSquare( 84, 220, Resource::Science, false);
		AddSquare(296, 288, Resource::Materials, true);
		SetWormholes("111101");
		m_nVictory = 2, m_bArtifact = true, nAncients = 1;
		break;
	case 106:
		AddSquare(167, 144, Resource::Science, false);
		AddSquare(267, 125, Resource::Materials, false);
		SetWormholes("111100");
		m_nVictory = 2;
		break;
	case 107:
		AddSquare(149, 252, Resource::Money, false);
		AddSquare(243, 123, Resource::Science, true);
		AddSquare(253, 269, Resource::Materials, true);
		SetWormholes("111101");
		m_nVictory = 2;
		break;
	case 108:
		AddSquare(135, 143, Resource::Money, true);
		AddSquare(247,  84, Resource::Science, false);
		AddSquare(273, 316, Resource::Any, false);
		SetWormholes("110110");
		m_nVictory = 2, nAncients = 2;
		break;
	case 201:
		AddSquare(145, 241, Resource::Money, false);
		AddSquare(204, 123, Resource::Materials, false);
		SetWormholes("010101");
		m_nVictory = 1;
		break;
	case 202:
		AddSquare(186, 311, Resource::Science, false);
		AddSquare(186, 260, Resource::Science, true);
		SetWormholes("010101");
		m_nVictory = 1;
		break;
	case 203:
		AddSquare(158, 306, Resource::Money, false);
		AddSquare(285, 298, Resource::Science, false);
		AddSquare(182,  87, Resource::Materials, false);
		SetWormholes("110101");
		m_nVictory = 1, nAncients = 2;
		break;
	case 204:
		AddSquare(166, 76, Resource::Money, true);
		AddSquare(174, 300, Resource::Materials, true);
		AddSquare(294, 116, Resource::Any, false);
		SetWormholes("110101");
		m_nVictory = 2, nAncients = 1, m_bArtifact  = true;
		break;
	case 205:
		AddSquare(171, 116, Resource::Money, false);
		AddSquare(223, 116, Resource::Money, true);
		AddSquare(253, 271, Resource::Science, true);
		SetWormholes("001110");
		m_nVictory = 1;
		break;
	case 206:
		AddSquare(167, 116, Resource::Materials, false);
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
		AddSquare(270, 108, Resource::Money, true);
		AddSquare(159, 191, Resource::Science, false);
		SetWormholes("110101");
		m_nVictory = 1;
		break;
	case 210:
		AddSquare(243,  99, Resource::Money, false);
		AddSquare(315, 253, Resource::Materials, false);
		SetWormholes("100101");
		m_nVictory = 1;
		break;
	case 211:
		AddSquare(283, 100, Resource::Money, false);
		AddSquare(280, 307, Resource::Materials, true);
		AddSquare(103, 261, Resource::Any, false);
		SetWormholes("111100");
		m_nVictory = 2, nAncients = 1, m_bArtifact  = true;
		break;

	case 221:
		AddSquare(191, 102, Resource::Money, false);
		AddSquare(247, 102, Resource::Money, true);
		AddSquare(184, 294, Resource::Science, false);
		AddSquare(133, 294, Resource::Science, true);
		AddSquare(320, 291, Resource::Materials, false);
		break;
	case 222:
		AddSquare(198, 106, Resource::Money, false);
		AddSquare(250, 106, Resource::Money, true);
		AddSquare(268, 274, Resource::Science, false);
		AddSquare(318, 274, Resource::Science, true);
		break;
	case 223:
		AddSquare(212, 105, Resource::Money, false);
		AddSquare(271, 105, Resource::Money, true);
		AddSquare(156, 192, Resource::Science, false);
		AddSquare( 98, 192, Resource::Science, true);
		AddSquare(299, 271, Resource::Materials, false);
		break;
	case 224:
		AddSquare(221, 111, Resource::Money, false);
		AddSquare(156, 192, Resource::Science, false);
		AddSquare( 98, 192, Resource::Science, true);
		AddSquare(258, 279, Resource::Materials, true);
		break;
	case 225:
		AddSquare(169, 309, Resource::Money, false);
		AddSquare(169, 256, Resource::Money, true);
		AddSquare(285, 262, Resource::Science, false);
		AddSquare(336, 262, Resource::Science, true);
		AddSquare(169, 124, Resource::Materials, false);
		break;
	case 226:
		AddSquare(191, 107, Resource::Money, false);
		AddSquare(158, 294, Resource::Science, false);
		AddSquare(342, 262, Resource::Materials, false);
		break;
	case 227:
		AddSquare(276, 285, Resource::Money, false);
		AddSquare(333, 285, Resource::Money, true);
		AddSquare(224, 118, Resource::Science, false);
		AddSquare(224,  69, Resource::Science, true);
		AddSquare(126, 250, Resource::Materials, false);
		break;
	case 228:
		AddSquare(183, 110, Resource::Money, false);
		AddSquare(169, 263, Resource::Science, false);
		AddSquare(291, 282, Resource::Materials, true);
		break;
	case 229:
		AddSquare(132, 274, Resource::Money, false);
		AddSquare(188, 274, Resource::Money, true);
		AddSquare(278, 273, Resource::Science, false);
		AddSquare(330, 273, Resource::Science, true);
		AddSquare(289,  79, Resource::Materials, false);
		break;
	case 230:
		AddSquare(180, 107, Resource::Money, false);
		AddSquare(235, 107, Resource::Money, true);
		AddSquare(146, 285, Resource::Science, false);
		AddSquare(341, 262, Resource::Materials, true);
		break;
	case 231:
		AddSquare(277, 265, Resource::Money, false);
		AddSquare(335, 265, Resource::Money, true);
		AddSquare(252, 112, Resource::Science, false);
		AddSquare(194, 112, Resource::Science, true);
		AddSquare(155, 279, Resource::Materials, false);
		break;
	case 232:
		AddSquare(230, 106, Resource::Money, true);
		AddSquare(131, 210, Resource::Science, false);
		AddSquare(310, 276, Resource::Materials, false);
		AddSquare(251, 276, Resource::Materials, true);
		break;

	case 301:
		AddSquare(302, 102, Resource::Money, false);
		AddSquare(112, 182, Resource::Science, false);
		AddSquare(294, 218, Resource::Materials, true);
		SetWormholes("101100");
		m_nVictory = 2, nAncients = 2, m_bArtifact  = true;
		break;
	case 302:
		AddSquare(277,  94, Resource::Money, true);
		AddSquare(305, 304, Resource::Materials, false);
		SetWormholes("100110");
		m_nVictory = 2, nAncients = 1, m_bArtifact  = true;
		break;
	case 303:
		AddSquare(215, 86, Resource::Any, false);
		SetWormholes("000101");
		m_nVictory = 2, nAncients = 1, m_bArtifact  = true;
		break;
	case 304:
		AddSquare(165, 126, Resource::Money, true);
		AddSquare(216, 281, Resource::Materials, false);
		SetWormholes("100100");
		m_nVictory = 2;
		break;
	case 305:
		AddSquare(133, 110, Resource::Science, false);
		AddSquare(260, 299, Resource::Materials, false);
		SetWormholes("110100");
		m_nVictory = 1, nAncients = 1;
		break;
	case 306:
		AddSquare(260, 113, Resource::Money, false);
		AddSquare(269, 284, Resource::Materials, false);
		SetWormholes("010100");
		m_nVictory = 1;
		break;
	case 307:
		AddSquare(219,  81, Resource::Money, false);
		AddSquare(146, 251, Resource::Science, true);
		SetWormholes("101100");
		m_nVictory = 1;
		break;
	case 308:
		AddSquare(283, 118, Resource::Science, false);
		AddSquare(155, 197, Resource::Materials, true);
		SetWormholes("001101");
		m_nVictory = 1;
		break;
	case 309:
		AddSquare(254, 126, Resource::Money, false);
		AddSquare(145, 207, Resource::Science, true);
		SetWormholes("100101");
		m_nVictory = 1;
		break;
	case 310:
		AddSquare(264,  76, Resource::Science, false);
		AddSquare(156, 248, Resource::Materials, false);
		SetWormholes("100100");
		m_nVictory = 1;
		break;
	case 311:
		AddSquare(288, 274, Resource::Materials, false);
		SetWormholes("101100");
		m_nVictory = 1, bDiscovery = true;
		break;
	case 312:
		AddSquare(251, 97, Resource::Materials, false);
		SetWormholes("110100");
		m_nVictory = 1, bDiscovery = true;
		break;
	case 313:
		AddSquare(263, 101, Resource::Any, false);
		SetWormholes("100100");
		m_nVictory = 1, bDiscovery = true;
		break;
	case 314:
		AddSquare(101, 244, Resource::Any, false);
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
		AddSquare(167, 265, Resource::Money, false);
		AddSquare(167, 316, Resource::Money, true);
		SetWormholes("000110");
		m_nVictory = 1;
		break;
	case 318:
		AddSquare(113, 236, Resource::Materials, true);
		AddSquare(221, 106, Resource::Any, false);
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
		AddShip(ShipType::Ancient, nullptr);

	bDiscovery |= nAncients > 0;

	if (bDiscovery && pGame)
		m_discovery = pGame->GetDiscoveryBag().TakeTile();
}
