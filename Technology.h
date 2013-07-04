#pragma once

enum class TechType {	
	None = -1,
	NeutronBomb, StarBase,	   PlasmaCannon, PhaseShield,  AdvMining,  TachyonSource, PlasmaMissile,    GluonComp,
	GaussShield, ImprovedHull, FusionSource, PositronComp, AdvEconomy, TachyonDrive,  AntimatterCannon, QuantumGrid,
	NanoRobots,	 FusionDrive,  AdvRobotics,  Orbital,      AdvLabs,    Monolith,      ArtifactKey,      WormholeGen  };

class Technology
{
public:
	enum class Class { Military, Grid, Nano, _Count };

	static int GetTier(TechType t);
	static Class GetClass(TechType t);
	
	static int GetMaxCost(TechType t);
	static int GetMinCost(TechType t);

private:
	TechType m_type;
};
