#pragma once

#include "MapPos.h"
#include "EdgeSet.h"

#include <vector>
#include <map>
#include <bitset>

enum class TechType;
enum class SquareType;

class SquareDef
{
public:
	SquareDef(int x, int y, SquareType type, bool bAdvanced);
	TechType GetRequiredTech() const;
	SquareType GetType() const { return m_type; }

	int GetX() const { return m_x; }
	int GetY() const { return m_y; }

private:
	int m_x, m_y; // Centre (pixels from TL of hex image).
	SquareType m_type;
	bool m_bAdvanced;
};

class HexDef
{
	friend class HexDefs;
public:
	int GetSquareCount() const { return (int)m_squares.size(); }
	const SquareDef& GetSquare(int i) const { return m_squares[i]; }
	
	EdgeSet GetWormholes() const { return m_wormholes; } 
	int GetVictoryPoints() const { return m_nVictory; }
	int GetAncients() const { return m_nAncients; }
	bool HasArtifact() const { return m_bArtifact; }
	bool HasDiscovery() const { return m_bDiscovery; }

	void SetArtifact(bool b) { m_bArtifact = b; }
	void SetDiscovery(bool b) { m_bDiscovery = b; }
	void SetAncients(int n) { m_nAncients = n; m_bDiscovery |= n > 0; }

private:
	HexDef(std::string s, int nVictory);
	void AddSquare(int x, int y, SquareType type, bool bAdvanced = false);

	std::vector<SquareDef> m_squares;
	EdgeSet m_wormholes;
	int m_nVictory;
	int m_nAncients;
	bool m_bArtifact;
	bool m_bDiscovery;
};

class HexDefs
{
public:
	static const HexDef& Get(int id);
	static HexRing GetRingFromId(int id);
	
private:
	HexDefs();

	HexDef* AddHex(int id, std::string s, int nVictory = 1);
	HexDef* AddStartHex(int id);

	static const HexDefs& Instance();

	std::map<int, HexDef> m_map;
};
