#pragma once

#include <map>
#include <vector>

namespace Serial { class SaveNode; class LoadNode; }

enum class Colour;
class Map;

class HexArrivals : public std::map<int, std::vector<Colour>> 
{
public:
	void MoveOwningTeamsToStart(const Map& map);
		
	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);
};
