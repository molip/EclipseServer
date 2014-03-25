#include "HexArrivals.h"
#include "Serial.h"
#include "Map.h"
#include "Team.h"

/* "If the hex already has an Influence Disc and is thus controlled 
by a player, that player is always considered the defender
and will fight last regardless of the entry order" */
void HexArrivals::MoveOwningTeamsToStart(const Map& map)
{
	for (auto& hexVecPair : *this)
	{
		const Hex* hex = map.FindHex(hexVecPair.first);
		VerifyModel("HexArrivals::MoveOwningTeamsToStart", hex != nullptr);
		const Colour colour = hex->GetColour();

		if (colour != Colour::None)
		{
			auto& vec = hexVecPair.second;
			auto it = std::find(vec.begin(), vec.end(), colour);
			if (it != vec.end())
			{
				vec.erase(it);
				vec.insert(vec.begin(), colour);
			}
		}
	}
}

void HexArrivals::Save(Serial::SaveNode& node) const
{
	node.SaveMap("map", *this, Serial::TypeSaver(), Serial::CntrSaver<Serial::EnumSaver>());
}

void HexArrivals::Load(const Serial::LoadNode& node)
{
	node.LoadMap("map", *this, Serial::TypeLoader(), Serial::CntrLoader<Serial::EnumLoader>());
}

