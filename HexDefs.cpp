#include "stdafx.h"
#include "HexDefs.h"
#include "Hex.h"
#include "Technology.h"

SquareDef::SquareDef(int x, int y, SquareType type, bool bAdvanced) : m_x(x), m_y(y), m_type(type), m_bAdvanced(bAdvanced)
{
}

TechType SquareDef::GetRequiredTech() const
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

HexDef::HexDef(std::string s, int nVictory) : 
	m_wormholes(s), m_nVictory(nVictory), m_nAncients(0), m_bArtifact(false), m_bDiscovery(false)
{
}

void HexDef::AddSquare(int x, int y, SquareType type, bool bAdvanced) 
{
	m_squares.push_back(SquareDef(x, y, type, bAdvanced)); 
}

//-----------------------------------------------------------------------------

const HexDefs& HexDefs::Instance()
{
	static HexDefs defs;
	return defs;
}

const HexDef& HexDefs::Get(int id)
{
	auto it = Instance().m_map.find(id);
	VERIFY_MODEL(it != Instance().m_map.end());
	return it->second;
}

HexDef* HexDefs::AddHex(int id, std::string s, int nVictory)
{
	auto it = m_map.insert(std::make_pair(id, HexDef(s, nVictory)));
	return &it.first->second;
}

HexDef* HexDefs::AddStartHex(int id)
{
	HexDef* p = AddHex(id, "110110", 3);
	p->SetArtifact(true);
	return p;
}

HexDefs::HexDefs()
{
	HexDef* p;
	p = AddHex(1, "111111", 4); 
	p->SetArtifact(true);
	p->SetDiscovery(true);
	p->AddSquare(299,  92, SquareType::Science);
	p->AddSquare(250,  92, SquareType::Science, true);
	p->AddSquare( 95, 235, SquareType::Materials);
	p->AddSquare( 95, 176, SquareType::Materials, true);
	p->AddSquare(218, 328, SquareType::Any);
	p->AddSquare(270, 328, SquareType::Any);
	//AddShip(ShipType::GCDS, Colour::None);

	p = AddHex(101, "011111", 2);
	p->SetAncients(1);
	p->AddSquare(310, 113, SquareType::Money);
	p->AddSquare(134,  87, SquareType::Materials);
	p->AddSquare(182,  87, SquareType::Materials, true);
	
	p = AddHex(102, "101101", 3);
	p->SetArtifact(true);
	p->AddSquare(138, 266, SquareType::Science);
	p->AddSquare(205, 266, SquareType::Science, true);

	p = AddHex(103, "111011", 2);
	p->AddSquare(195, 271, SquareType::Money, true);
	p->AddSquare(286, 293, SquareType::Any);

	p = AddHex(104, "110110", 2);
	p->SetAncients(2);
	p->AddSquare(322, 104, SquareType::Money);
	p->AddSquare(275, 104, SquareType::Money, true);
	p->AddSquare(124, 287, SquareType::Science);
	p->AddSquare(124, 238, SquareType::Science, true);

	p = AddHex(105, "111101", 3);
	p->SetAncients(1);
	p->SetArtifact(true);
	p->AddSquare(281,  83, SquareType::Money);
	p->AddSquare( 84, 220, SquareType::Science);
	p->AddSquare(296, 288, SquareType::Materials, true);

	p = AddHex(106, "111100", 2);
	p->AddSquare(167, 144, SquareType::Science);
	p->AddSquare(267, 125, SquareType::Materials);
		
	p = AddHex(107, "111101", 2);
	p->AddSquare(149, 252, SquareType::Money);
	p->AddSquare(243, 123, SquareType::Science, true);
	p->AddSquare(253, 269, SquareType::Materials, true);

	p = AddHex(108, "110110", 2);
	p->SetAncients(2);
	p->AddSquare(135, 143, SquareType::Money, true);
	p->AddSquare(247,  84, SquareType::Science);
	p->AddSquare(273, 316, SquareType::Any);

	p = AddHex(201, "010101");
	p->AddSquare(145, 241, SquareType::Money);
	p->AddSquare(204, 123, SquareType::Materials);

	p = AddHex(202, "010101");
	p->AddSquare(186, 311, SquareType::Science);
	p->AddSquare(186, 260, SquareType::Science, true);

	p = AddHex(203, "110101");
	p->SetAncients(2);
	p->AddSquare(158, 306, SquareType::Money);
	p->AddSquare(285, 298, SquareType::Science);
	p->AddSquare(182,  87, SquareType::Materials);

	p = AddHex(204, "110101", 2);
	p->SetAncients(1);
	p->SetArtifact(true);
	p->AddSquare(166, 76, SquareType::Money, true);
	p->AddSquare(174, 300, SquareType::Materials, true);
	p->AddSquare(294, 116, SquareType::Any);

	p = AddHex(205, "001110");
	p->AddSquare(171, 116, SquareType::Money);
	p->AddSquare(223, 116, SquareType::Money, true);
	p->AddSquare(253, 271, SquareType::Science, true);
		
	p = AddHex(206, "011101");
	p->SetDiscovery(true);
	p->AddSquare(167, 116, SquareType::Materials);

	p = AddHex(207, "110100");
	p->SetDiscovery(true);

	p = AddHex(208, "101101");
	p->SetDiscovery(true);

	p = AddHex(209, "110101");
	p->AddSquare(270, 108, SquareType::Money, true);
	p->AddSquare(159, 191, SquareType::Science);

	p = AddHex(210, "100101");
	p->AddSquare(243,  99, SquareType::Money);
	p->AddSquare(315, 253, SquareType::Materials);

	p = AddHex(211, "111100", 2);
	p->SetAncients(1);
	p->SetArtifact(true);
	p->AddSquare(283, 100, SquareType::Money);
	p->AddSquare(280, 307, SquareType::Materials, true);
	p->AddSquare(103, 261, SquareType::Any);
		
	p = AddStartHex(221);
	p->AddSquare(191, 102, SquareType::Money);
	p->AddSquare(247, 102, SquareType::Money, true);
	p->AddSquare(184, 294, SquareType::Science);
	p->AddSquare(133, 294, SquareType::Science, true);
	p->AddSquare(320, 291, SquareType::Materials);

	p = AddStartHex(222);
	p->AddSquare(198, 106, SquareType::Money);
	p->AddSquare(250, 106, SquareType::Money, true);
	p->AddSquare(268, 274, SquareType::Science);
	p->AddSquare(318, 274, SquareType::Science, true);
		
	p = AddStartHex(223);
	p->AddSquare(212, 105, SquareType::Money);
	p->AddSquare(271, 105, SquareType::Money, true);
	p->AddSquare(156, 192, SquareType::Science);
	p->AddSquare( 98, 192, SquareType::Science, true);
	p->AddSquare(299, 271, SquareType::Materials);
		
	p = AddStartHex(224);
	p->AddSquare(212, 112, SquareType::Money);
	p->AddSquare(156, 192, SquareType::Science);
	p->AddSquare( 98, 192, SquareType::Science, true);
	p->AddSquare(258, 279, SquareType::Materials, true);
		
	p = AddStartHex(225);
	p->AddSquare(169, 309, SquareType::Money);
	p->AddSquare(169, 256, SquareType::Money, true);
	p->AddSquare(285, 262, SquareType::Science);
	p->AddSquare(336, 262, SquareType::Science, true);
	p->AddSquare(169, 124, SquareType::Materials);
		
	p = AddStartHex(226);
	p->AddSquare(191, 107, SquareType::Money);
	p->AddSquare(158, 294, SquareType::Science);
	p->AddSquare(342, 262, SquareType::Materials);
		
	p = AddStartHex(227);
	p->AddSquare(276, 285, SquareType::Money);
	p->AddSquare(333, 285, SquareType::Money, true);
	p->AddSquare(224, 118, SquareType::Science);
	p->AddSquare(224,  69, SquareType::Science, true);
	p->AddSquare(126, 250, SquareType::Materials);
		
	p = AddStartHex(228);
	p->AddSquare(183, 110, SquareType::Money);
	p->AddSquare(169, 263, SquareType::Science);
	p->AddSquare(291, 282, SquareType::Materials, true);
		
	p = AddStartHex(229);
	p->AddSquare(132, 274, SquareType::Money);
	p->AddSquare(188, 274, SquareType::Money, true);
	p->AddSquare(278, 273, SquareType::Science);
	p->AddSquare(330, 273, SquareType::Science, true);
	p->AddSquare(289,  79, SquareType::Materials);
		
	p = AddStartHex(230);
	p->AddSquare(180, 107, SquareType::Money);
	p->AddSquare(235, 107, SquareType::Money, true);
	p->AddSquare(146, 285, SquareType::Science);
	p->AddSquare(341, 262, SquareType::Materials, true);
		
	p = AddStartHex(231);
	p->AddSquare(277, 265, SquareType::Money);
	p->AddSquare(335, 265, SquareType::Money, true);
	p->AddSquare(252, 112, SquareType::Science);
	p->AddSquare(194, 112, SquareType::Science, true);
	p->AddSquare(155, 279, SquareType::Materials);
	
	p = AddStartHex(232);
	p->AddSquare(230, 106, SquareType::Money, true);
	p->AddSquare(131, 210, SquareType::Science);
	p->AddSquare(310, 276, SquareType::Materials);
	p->AddSquare(251, 276, SquareType::Materials, true);
		
	p = AddHex(301, "101100", 2);
	p->SetAncients(2);
	p->SetArtifact(true);
	p->AddSquare(302, 102, SquareType::Money);
	p->AddSquare(112, 182, SquareType::Science);
	p->AddSquare(294, 218, SquareType::Materials, true);
		
	p = AddHex(302, "100110", 2);
	p->SetAncients(1);
	p->SetArtifact(true);
	p->AddSquare(277,  94, SquareType::Money, true);
	p->AddSquare(305, 304, SquareType::Materials);
		
	p = AddHex(303, "000101", 2);
	p->SetAncients(1);
	p->SetArtifact(true);
	p->AddSquare(215, 86, SquareType::Any);
		
	p = AddHex(304, "100100");
	p->AddSquare(165, 126, SquareType::Money, true);
	p->AddSquare(216, 281, SquareType::Materials);
		
	p = AddHex(305, "110100");
	p->SetAncients(1);
	p->AddSquare(133, 110, SquareType::Science);
	p->AddSquare(260, 299, SquareType::Materials);
		
	p = AddHex(306, "010100");
	p->AddSquare(260, 113, SquareType::Money);
	p->AddSquare(269, 284, SquareType::Materials);
		
	p = AddHex(307, "101100");
	p->AddSquare(219,  81, SquareType::Money);
	p->AddSquare(146, 251, SquareType::Science, true);
		
	p = AddHex(308, "001101");
	p->AddSquare(283, 118, SquareType::Science);
	p->AddSquare(155, 197, SquareType::Materials, true);
		
	p = AddHex(309, "100101");
	p->AddSquare(254, 126, SquareType::Money);
	p->AddSquare(145, 207, SquareType::Science, true);
		
	p = AddHex(310, "100100");
	p->AddSquare(264,  76, SquareType::Science);
	p->AddSquare(156, 248, SquareType::Materials);
		
	p = AddHex(311, "101100");
	p->SetDiscovery(true);
	p->AddSquare(288, 274, SquareType::Materials);
		
	p = AddHex(312, "110100");
	p->SetDiscovery(true);
	p->AddSquare(251, 97, SquareType::Materials);
		
	p = AddHex(313, "100100");
	p->SetDiscovery(true);
	p->AddSquare(263, 101, SquareType::Any);
		
	p = AddHex(314, "001110");
	p->SetDiscovery(true);
	p->AddSquare(101, 244, SquareType::Any);
		
	p = AddHex(315, "100101");
	p->SetDiscovery(true);
		
	p = AddHex(316, "110100");
	p->SetDiscovery(true);
		
	p = AddHex(317, "000110");
	p->AddSquare(167, 265, SquareType::Money);
	p->AddSquare(167, 316, SquareType::Money, true);
		
	p = AddHex(318, "001100");
	p->AddSquare(113, 236, SquareType::Materials, true);
	p->AddSquare(221, 106, SquareType::Any);
}

