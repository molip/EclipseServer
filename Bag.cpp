#include "stdafx.h"
#include "Bag.h"
#include "App.h"
#include "Game.h"

#include <numeric>

void DiscoveryBag::Init()
{
	for (int i = 0; i < 3; ++i)
	{
		m_vec.push_back(DiscoveryType::Money);
		m_vec.push_back(DiscoveryType::Science);
		m_vec.push_back(DiscoveryType::Materials);
		m_vec.push_back(DiscoveryType::AncientTech);
		m_vec.push_back(DiscoveryType::AncientCruiser);
	}
	m_vec.push_back(DiscoveryType::AxionComputer);
	m_vec.push_back(DiscoveryType::HypergridSource);
	m_vec.push_back(DiscoveryType::ShardHull);
	m_vec.push_back(DiscoveryType::IonTurret);
	m_vec.push_back(DiscoveryType::ConformalDrive);
	m_vec.push_back(DiscoveryType::FluxShield);

	std::shuffle(m_vec.begin(), m_vec.end(), GetRandom());
}

void TechnologyBag::Init()
{
	
	TechType fives[] = {	TechType::NeutronBomb,		TechType::StarBase,			TechType::PlasmaCannon, 
							TechType::GaussShield,		TechType::ImprovedHull,		TechType::FusionSource, 
							TechType::NanoRobots,		TechType::FusionDrive,		TechType::AdvRobotics, };
	TechType fours[] = {	TechType::PhaseShield,		TechType::AdvMining,		TechType::PositronComp, 
							TechType::AdvEconomy,		TechType::Orbital,			TechType::AdvLabs };
	TechType threes[] = {	TechType::TachyonSource,	TechType::PlasmaMissile,	TechType::GluonComp, 
							TechType::TachyonDrive,		TechType::AntimatterCannon, TechType::QuantumGrid, 
							TechType::Monolith,			TechType::ArtifactKey,		TechType::WormholeGen, };

	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < _countof(fives); ++j)
			m_vec.push_back(fives[j]);

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < _countof(fours); ++j)
			m_vec.push_back(fours[j]);

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < _countof(threes); ++j)
			m_vec.push_back(threes[j]);

	std::shuffle(m_vec.begin(), m_vec.end(), GetRandom());
}

HexPile HexPile::Create(HexRing r, int nPlayers)
{
	HexPile pile;

	VERIFY_MODEL(nPlayers > 0 && nPlayers < 7);

	int start = 0, count = 0;
	switch (r)
	{
	case HexRing::Inner: start = 101, count	= 8; break;
	case HexRing::Middle: start = 201, count = 11; break;
	case HexRing::Outer: start = 301, count	= 18; break;
	}

	for (int i = start; i < start + count; ++i)
		pile.m_vec.push_back(i);

	std::shuffle(pile.m_vec.begin(), pile.m_vec.end(), GetRandom());

	if (r == HexRing::Outer)
	{
		const int outers[] = { 5, 5, 10, 14, 16, 18 };
		pile.m_vec.resize(outers[nPlayers - 1]);
	}
	return pile;
}

int HexPile::TakeTile()
{
	VERIFY_MODEL(!m_vec.empty());
	int tile = m_vec.back();
	m_vec.pop_back();
	return tile;
}

void HexPile::ReturnTile(int tile)
{
	m_vec.push_back(tile);
}

ReputationBag::ReputationBag() : m_counts { 12, 9, 7, 4 }
{
}

int ReputationBag::GetTileCount() const
{
	return std::accumulate(m_counts.begin(), m_counts.end(), 0);
}

int ReputationBag::ChooseBestTile(int count) const
{
	VERIFY(count >= 1 && count <= 5);

	auto counts = m_counts;

	int tileCount = GetTileCount();
	VERIFY_MODEL(tileCount > 0);
	count = std::min(count, tileCount);

	int best = 0;

	for (int i = 0; i < count; ++i)
	{
		// Choose a random tile. 
		int index = ::GetRandom()() % tileCount;
		int val = 0;
		for (;; ++val)
		{
			index -= counts[val];
			if (index < 0)
				break;
		}

		--counts[val];
		--tileCount;
		best = std::max(best, val);
	}

	return best + 1;
}

int ReputationBag::ChooseAndTakeTile()
{
	int val = ChooseBestTile(1);
	TakeTile(val);
	return val;
}

void ReputationBag::TakeTile(int val)
{
	VERIFY_MODEL(m_counts[val - 1] > 0);
	--m_counts[val - 1];
}

void ReputationBag::ReturnTile(int val)
{
	++m_counts[val - 1];
}

void ReputationBag::Save(Serial::SaveNode& node) const
{
	node.SaveArray("counts", m_counts, Serial::TypeSaver());
}

void ReputationBag::Load(const Serial::LoadNode& node)
{
	node.LoadArray("counts", m_counts, Serial::TypeLoader());
}
