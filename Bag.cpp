#include "stdafx.h"
#include "Bag.h"
#include "App.h"
#include "Game.h"

DiscoveryBag::DiscoveryBag()
{
	for (int i = 0; i < 3; ++i)
	{
		push_back(DiscoveryType::Money);
		push_back(DiscoveryType::Science);
		push_back(DiscoveryType::Materials);
		push_back(DiscoveryType::AncientTech);
		push_back(DiscoveryType::AncientCruiser);
	}
	push_back(DiscoveryType::AxiomComputer);
	push_back(DiscoveryType::HypergridSource);
	push_back(DiscoveryType::ShardHull);
	push_back(DiscoveryType::IonTurret);
	push_back(DiscoveryType::ConformalDrive);
	push_back(DiscoveryType::FluxShield);

	std::random_shuffle(begin(), end());
}

ReputationBag::ReputationBag()
{
	const int num[] = { 12, 9, 7, 4 };

	for (int val = 1; val <= 4; ++val)
		for (int i = 0; i < num[val - 1]; ++i)
			push_back(val);

	std::random_shuffle(begin(), end());
}

TechnologyBag::TechnologyBag()
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
			push_back(fives[j]);

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < _countof(fours); ++j)
			push_back(fours[j]);

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < _countof(threes); ++j)
			push_back(threes[j]);

	std::random_shuffle(begin(), end());
}

HexBag::HexBag(HexRing r, int nPlayers)
{
	AssertThrowModel("HexBag::HexBag", nPlayers > 0 && nPlayers < 7);

	int start = 0, count = 0;
	switch (r)
	{
	case HexRing::Inner: start = 101, count	= 8; break;
	case HexRing::Middle: start = 201, count = 11; break;
	case HexRing::Outer: start = 301, count	= 18; break;
	}

	for (int i = start; i < start + count; ++i)
		push_back(i);

	std::random_shuffle(begin(), end());

	if (r == HexRing::Outer)
	{
		const int outers[] = { 5, 5, 10, 14, 16, 18 };
		resize(outers[nPlayers - 1]);
	}
}
