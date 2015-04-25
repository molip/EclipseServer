#include "Dice.h"
#include "App.h"
#include "Serial.h"

Dice::Dice()
{
}

void Dice::Add(DiceColour colour, int count)
{
	for (int i = 0; i < count; ++i)
		operator[](colour).insert(1 + (GetRandom()() % 6));
}

Dice Dice::GetExactDiceForDamage(int damage, int toHit) const
{
	Dice dice;

	for (auto& colourRollsPair : *this)
	{
		const auto colour = colourRollsPair.first;
		auto& rolls = colourRollsPair.second;

		const int colourDamage = GetDamage(colour);
		for (int roll : colourRollsPair.second)
		{
			if (damage < colourDamage)
				break;

			if (roll >= toHit)
			{
				damage -= colourDamage;
				dice[colour].insert(roll);
				if (damage == 0)
					return dice;
			}
		}
	}
	return Dice();
};

Dice Dice::Remove(int damage, int toHit)
{
	const int total = GetDamage(toHit);
	for (; damage <= total; ++damage)
	{
		Dice dice = GetExactDiceForDamage(damage, toHit);
		if (!dice.empty())
		{
			ASSERT(Remove(dice) == dice.GetCount());
			return dice;
		}
	}
	return Dice();
}

Dice Dice::RemoveAll(int toHit)
{
	Dice dice;
	for (auto& colourRollsPair : *this)
		for (int roll : colourRollsPair.second)
			if (roll >= toHit)
				dice[colourRollsPair.first].insert(roll);
	
	ASSERT(Remove(dice) == dice.GetCount());
	return dice;
}

int Dice::Remove(const Dice& dice)
{
	int removed = 0;
	for (auto& colourRollsPair : dice)
	{
		auto colourIt = find(colourRollsPair.first);
		if (colourIt != end())
		{
			auto& rolls = colourIt->second;
			for (int roll : colourRollsPair.second)
			{
				auto rollIt = rolls.find(roll);
				if (rollIt != rolls.end())
				{
					++removed;
					rolls.erase(rollIt);
					if (rolls.empty())
					{
						erase(colourIt);
						break;
					}
				}
			}
		}
	}
	return removed;
}

int Dice::GetDamage(int toHit) const
{
	int damage = 0;

	for (auto& colourRollsPair : *this)
		for (int roll : colourRollsPair.second)
			if (roll >= toHit)
				damage += GetDamage(colourRollsPair.first);

	return damage;
}

int Dice::GetCount() const
{
	int count = 0;
	for (auto& colourRollsPair : *this)
		count += (int)colourRollsPair.second.size();
	return count;
}

int Dice::GetDamage(DiceColour colour)
{
	switch (colour)
	{
	case DiceColour::Yellow:	return 1;
	case DiceColour::Orange:	return 2;
	case DiceColour::Red:		return 4;
	}
	ASSERT(false);
	return 0;
}

int Dice::GetToHitRoll(int computer, int shield)
{
	return std::min(6, std::max(2, 6 - computer + shield));
}

void Dice::Save(Serial::SaveNode& node) const
{
	using namespace Serial;

	node.SaveMap("map", *this, EnumSaver(), CntrSaver<TypeSaver>());
}

void Dice::Load(const Serial::LoadNode& node)
{
	using namespace Serial;

	node.LoadMap("map", *this, EnumLoader(), CntrLoader<TypeLoader>());
}

DEFINE_ENUM_NAMES(DiceColour) { "Yellow", "Orange", "Red", "" };
