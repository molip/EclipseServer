#pragma once

class Technology
{
public:
	enum class Class { Military, Grid, Nano, _Count };
	enum class Type {	
		NeutronBomb, StarBase,	   PlasmaCannon, PhaseShield,  AdvMining,  TachyonSource, PlasmaMissile,    GluonComp,
		GaussShield, ImprovedHull, FusionSource, PositronComp, AdvEconomy, TachyonDrive,  AntimatterCannon, QuantumGrid,
		NanoRobots,	 FusionDrive,  AdvRobotics,  Orbital,      AdvLabs,    Monolith,      ArtifactKey,      WormholeGen  };

	Technology(Type t);
	~Technology();

	int GetTier() const;
	Class GetClass() const;
	
	static int GetMaxCost(int tier);
	static int GetMinCost(int tier);

private:
	Type m_type;
};

