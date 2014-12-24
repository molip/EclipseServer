#pragma once

#include <vector>
#include <map>
#include <set>

namespace Serial { class SaveNode; class LoadNode; }

enum class DiceColour { Yellow, Orange, Red };

class Dice : public std::map<DiceColour, std::multiset<int>>
{
public:
	Dice();

	void Add(DiceColour colour, int count);
	int Remove(const Dice& dice); // Returns number of dice removed. 
	Dice Remove(int damage, int toHit); // Remove dice needed to cause damage (largest first). Returned dice damage may be higher than specified. 
	Dice RemoveAll(int toHit); 
	
	int GetDamage(int toHit = 1) const;
	int GetCount() const;

	static int GetDamage(DiceColour colour);
	static int GetToHitRoll(int computer, int shield);

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);
private:
	Dice GetExactDiceForDamage(int damage, int toHit) const;
};
