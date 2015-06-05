#include "stdafx.h"
#include "Map.h"
#include "Hex.h"
#include "Game.h"
#include "App.h"
#include "EdgeSet.h"

Map::Map(Game& game) : m_game(game)
{
}

Map::Map(const Map& rhs, Game& game) : m_game(game)
{
	for (auto& h : rhs.m_hexes)
		m_hexes.insert(std::make_pair(h.first, HexPtr(new Hex(*h.second))));
}

bool Map::operator==(const Map& rhs) const
{
	return ArePtrMapsEqual(m_hexes, rhs.m_hexes);
}

Hex* Map::FindHex(int hexId) 
{
	for (auto& h : m_hexes)
		if (h.second->GetID() == hexId)
			return h.second.get();
	return nullptr;
}

Hex* Map::FindHex(const MapPos& pos)
{
	auto h = m_hexes.find(pos);
	return h == m_hexes.end() ? nullptr : h->second.get();
}

// Ignores hex IDs greater or equal to lastHex.
const Hex* Map::FindPendingBattleHex(const Game& game, int lastHex) const
{
	std::map<int, const Hex*> map;

	for (auto& h : m_hexes)
		if (lastHex == 0 || h.second->GetID() < lastHex)
			if (h.second->HasPendingBattle(game))
				map.insert(std::make_pair(h.second->GetID(), h.second.get()));

	return map.empty() ? nullptr : map.rbegin()->second;
}

// May still return true after battles are complete, if population wasn't destroyed. 
bool Map::HasPendingBattle(const Game& game) const
{
	for (auto& h : m_hexes)
		if (h.second->HasPendingBattle(game))
			return true;
	return false;
}

Hex& Map::GetHex(const MapPos& pos)
{
	Hex* pHex = FindHex(pos);
	VERIFY_MODEL(!!pHex);
	return *pHex;
}

std::vector<MapPos> Map::GetOwnedHexPositions(const Team& team) const
{
	std::vector<MapPos> result;
	for (auto& h : m_hexes)
		if (h.second->IsOwnedBy(team))
			result.push_back(h.first);
	return result;
}

Map::StartPositions Map::GetTeamStartPositions() const
{
	const int nTeams = (int)m_game.GetTeams().size();
	VERIFY_MODEL(nTeams > 0 && nTeams < 7);

	std::vector<int> rotations;
	std::string s;
	switch (nTeams)
	{
	case 6: s = "111111"; rotations = { 0, 1, 5, 0, 1, 5 }; break;
	case 5: s = "111011"; rotations = { 0, 0, 1, 1, 5 }; break;
	case 4: s = "011011"; rotations = { 1, 1, 1, 1 }; break;
	case 3: s = "101010"; rotations = { 0, 5, 1 }; break;
	case 2: s = "100100"; rotations = { 0, 0 }; break;
	case 1: s = "100000"; rotations = { 0 }; break;
	}

	std::vector<Edge> edges = EdgeSet(s).GetEdges();
	assert(edges.size() == nTeams);

	StartPositions positions;
	for (int i = 0; i < nTeams; ++i)
	{
		Edge e = edges[i];
		positions.push_back(std::make_pair(MapPos(0, 0).GetNeighbour(e).GetNeighbour(e), rotations[i]));
	}
	return positions;
}

Hex& Map::AddHex(HexPtr hex)
{
	Hex& hex2 = *hex;
	VERIFY_MODEL_MSG("hex already occupied", FindHex(hex->GetPos()) == nullptr);
	m_hexes.insert(std::make_pair(hex->GetPos(), std::move(hex)));
	return hex2;
}

void Map::DeleteHex(const MapPos& pos)
{
	auto i = m_hexes.find(pos);
	VERIFY_MODEL_MSG("hex not found", i != m_hexes.end());
	m_hexes.erase(i);
}

void Map::GetInfluencableNeighbours(const MapPos& pos, const Team& team, std::set<MapPos>& neighbours) const
{
	const bool bWormholeGen = team.HasTech(TechType::WormholeGen);

	const Hex& hex= GetHex(pos);
	VERIFY_MODEL_MSG("wrong owner", !hex.IsOwned() || hex.IsOwnedBy(team));

	for (auto e : EnumRange<Edge>())
		if (int nWormholes = hex.HasWormhole(e) + bWormholeGen)
		{
			MapPos pos2 = pos.GetNeighbour(e);
			if (const Hex* pHex2 = FindHex(pos2))
				if (!pHex2->IsOwned()) // "a hex that does not contain an Influence Disc..."
					if (nWormholes + pHex2->HasWormhole(ReverseEdge(e)) >= 2)
						if (!pHex2->HasForeignShip(team.GetColour())) // "...or an enemy Ship"
							neighbours.insert(pos2);
		}
}

void Map::GetEmptyNeighbours(const MapPos& pos, bool bWormholeGen, std::set<MapPos>& neighbours) const
{
	const Hex& hex = GetHex(pos);

	for (auto e : EnumRange<Edge>())
		if (bWormholeGen || hex.HasWormhole(e))
		{
			MapPos pos2 = pos.GetNeighbour(e);
			if (FindHex(pos2) == nullptr)
				if (!m_game.IsHexPileEmpty(pos2.GetRing()))
					neighbours.insert(pos2);
		}
}

std::set<MapPos> Map::GetNeighbours(const MapPos& pos, bool bWormholeGen) const
{
	std::set<MapPos> neighbours;

	const Hex& hex = GetHex(pos);

	for (auto e : EnumRange<Edge>())
		if (bWormholeGen || hex.HasWormhole(e))
		{
			MapPos pos2 = pos.GetNeighbour(e);
			if (FindHex(pos2))
				neighbours.insert(pos2);
		}
	return neighbours;
}

bool Map::HasNeighbour(const MapPos& pos, bool bWormholeGen) const
{
	const Hex& hex = GetHex(pos);

	for (auto e : EnumRange<Edge>())
		if (bWormholeGen || hex.HasWormhole(e))
		{
			MapPos pos2 = pos.GetNeighbour(e);
			if (FindHex(pos2))
				return true;
		}
	return false;
}

int Map::GetHexVictoryPoints(const Team & team) const
{
	int score = 0;
	for (auto& h : m_hexes)
		if (h.second->IsOwnedBy(team))
			score += h.second->GetVictoryPoints();
	return score;
}

int Map::GetMonolithVictoryPoints(const Team & team) const
{
	int score = 0;
	for (auto& h : m_hexes)
		if (h.second->IsOwnedBy(team))
			score += h.second->HasMonolith() * 3;
	return score;
}

int Map::GetRaceVictoryPoints(const Team & team) const
{
	const RaceType& race = team.GetRace();
	if (race != RaceType::Planta && race != RaceType::Descendants)
		return 0;

	int score = 0;
	for (auto& h : m_hexes)
		if (race == RaceType::Planta)
			score += h.second->IsOwnedBy(team);
		else if (race == RaceType::Descendants)
			score += h.second->GetShipCount(Colour::None, ShipType::Ancient);
	return score;
}

std::vector<const Hex*> Map::GetValidExploreOriginNeighbours(const MapPos& pos, const Team& team) const
{
	std::vector<const Hex*> hexes;
	hexes.resize(6);

	for (auto e : EnumRange<Edge>())
		if (const Hex* pHex = FindHex(pos.GetNeighbour(e)))
			if (pHex->CanExploreFrom(team)) // TODO: Check ships.
				hexes[(int)e] = pHex;

	return hexes;		
}

void Map::Save(Serial::SaveNode& node) const
{
	node.SaveMap("hexes", m_hexes, Serial::TypeSaver(), Serial::ClassPtrSaver());
}

void Map::Load(const Serial::LoadNode& node)
{
	node.LoadMap("hexes", m_hexes, Serial::TypeLoader(), Serial::ClassPtrLoader());

	for (auto& kv : m_hexes)
		kv.second->SetPos(kv.first);
}
