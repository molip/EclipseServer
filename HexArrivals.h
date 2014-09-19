#pragma once

#include <map>
#include <vector>

namespace Serial { class SaveNode; class LoadNode; }

enum class Colour;
class Map;
class LiveGame;

class CombatSite : public std::vector<Colour>
{
	friend class HexArrivals;
public:
	CombatSite(int hexId = 0);
	void MoveOwningTeamsToStart(const Map& map);
	bool IsPeaceful(const LiveGame& game) const;

private:
	int m_hexId;
};

class HexArrivals : public std::map<int, CombatSite> 
{
public:
	void MoveOwningTeamsToStart(const Map& map);
	void RemovePeaceful(const LiveGame& game);

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);
};
