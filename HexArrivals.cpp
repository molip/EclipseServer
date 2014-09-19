#include "HexArrivals.h"
#include "Serial.h"
#include "Map.h"
#include "Team.h"
#include "LiveGame.h"

CombatSite::CombatSite(int hexId) : m_hexId(hexId)
{
}

/* "If the hex already has an Influence Disc and is thus controlled 
by a player, that player is always considered the defender
and will fight last regardless of the entry order" */
void CombatSite::MoveOwningTeamsToStart(const Map& map)
{
	const Hex* hex = map.FindHex(m_hexId);
	VerifyModel("CombatSite::MoveOwningTeamsToStart", hex != nullptr);
	const Colour colour = hex->GetColour();

	if (colour != Colour::None)
	{
		auto it = std::find(begin(), end(), colour);
		if (it != end())
		{
			erase(it);
			insert(begin(), colour);
		}
	}
}

bool CombatSite::IsPeaceful(const LiveGame& game) const
{
	if (size() < 2)
		return true;

	if (size() == 2)
	{
		const Hex* hex = game.GetMap().FindHex(m_hexId);
		VerifyModel("CombatSite::IsPeaceful 1", hex != nullptr);
		
		if (Race(game.GetTeam(back()).GetRace()).IsAncientsAlly() && hex->HasShip(Colour::None, ShipType::Ancient))
		{
			VerifyModel("CombatSite::IsPeaceful 2", front() == Colour::None); // Ancients should have been there first. 
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------

void HexArrivals::MoveOwningTeamsToStart(const Map& map)
{
	for (auto& pair : *this)
		pair.second.MoveOwningTeamsToStart(map);
}

void HexArrivals::RemovePeaceful(const LiveGame& game)
{
	for (auto it = begin(); it != end();)
		if (it->second.IsPeaceful(game))
			it = erase(it);
		else
			++it;
}

void HexArrivals::Save(Serial::SaveNode& node) const
{
	node.SaveMap("map", *this, Serial::TypeSaver(), Serial::CntrSaver<Serial::EnumSaver>());
}

void HexArrivals::Load(const Serial::LoadNode& node)
{
	node.LoadMap("map", *this, Serial::TypeLoader(), Serial::CntrLoader<Serial::EnumLoader>());

	for (auto& pair : *this)
		pair.second.m_hexId = pair.first;
}

