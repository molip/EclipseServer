#pragma once

#include "Resources.h"
#include "Reputation.h"
#include "Blueprint.h"

enum class RaceType { None = -1, Human, Eridani, Hydran, Planta, Descendants, Mechanema, Orion, _Count };
enum class Resource;
enum class ShipType;
enum class Colour;
enum class ReputationType;
enum class Buildable;
enum class TechType;

class Race
{
public:
	Race(RaceType type) : m_type(type) {}

	RaceType GetType() const { return m_type; }

	Storage GetStartStorage() const;
	int GetStartReputationTiles() const;
	int GetStartInfluenceDiscs() const;
	int GetStartColonyShips() const;
	ShipType GetStartShip() const;
	int GetStartSector(Colour colour) const;
	int GetMoves() const;
	ReputationSlots GetReputationSlots() const;
	BlueprintPtr GetBlueprint(ShipType type) const;
	int GetBuildCost(Buildable b) const;
	std::vector<TechType> GetStartTechnologies() const;
	int GetTradeRate() const;
	int GetResearchRate() const;
	int GetBuildRate() const;
	int GetUpgradeRate() const;
	int GetExploreRate() const;
	int GetExploreChoices() const;
	bool IsPopulationAutoDestroyed() const;
	int GetExtraVictoryPointsPerHex() const;
	bool IsAncientsAlly() const;
	Colour GetColour() const;

private:
	RaceType m_type;
};