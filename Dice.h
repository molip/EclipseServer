#pragma once

#include <vector>
#include <map>

namespace Serial { class SaveNode; class LoadNode; }

enum class DiceColour { Yellow, Orange, Red };

class Dice : public std::map<DiceColour, std::vector<int>>
{
public:
	Dice();

	void Add(DiceColour colour, int count);

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);
};
