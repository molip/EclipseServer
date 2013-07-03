#include "stdafx.h"
#include "Technology.h"
#include "App.h"
#include "EnumTraits.h"

int Technology::GetTier(TechType t) 
{
	return int(t) % 8;
}

Technology::Class Technology::GetClass(TechType t) 
{
	return Technology::Class((int)t / 8);
}

int Technology::GetMaxCost(int tier) 
{
	return (tier + 1) * 2;
}

int Technology::GetMinCost(int tier) 
{
	return tier < 5 ? tier + 2 : tier + 1;
}

DEFINE_ENUM_NAMES(Technology::Class) { "Military", "Grid", "Nano", "" };

DEFINE_ENUM_NAMES(TechType) { 
	"NeutronBomb",	"StarBase",		"PlasmaCannon", "PhaseShield",  "AdvMining",  "TachyonSource", "PlasmaMissile",    "GluonComp",
	"GaussShield",	"ImprovedHull", "FusionSource", "PositronComp", "AdvEconomy", "TachyonDrive",  "AntimatterCannon", "QuantumGrid",
	"NanoRobots",	"FusionDrive",  "AdvRobotics",  "Orbital",      "AdvLabs",    "Monolith",      "ArtifactKey",      "WormholeGen", "" };

