#include "Dice.h"
#include "App.h"
#include "Serial.h"

Dice::Dice()
{
}

void Dice::Add(DiceColour colour, int count)
{
	for (int i = 0; i < count; ++i)
		operator[](colour).push_back(1 + (GetRandom()() % 6));
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
