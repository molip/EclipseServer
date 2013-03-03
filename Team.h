#pragma once

#include "Technology.h"
#include "TechTrack.h"

#include <vector>
#include <memory>

enum class Resource { Money, Science, Materials, _Count };
enum class Race { Eridani, Hydran, Planta, Descendants, Mechanema, Orion, Human, _Count };
enum class Colour { Black, Blue, Green, Red, White, Yellow, _Count };

class Resources : private std::vector<int>
{
public:
	const int& operator[] (Resource r) const { return __super::at((int)r); }
	int& operator[] (Resource r) { return __super::at((int)r); }

protected:
	Resources()  { }

private:
	int m_array[Resource::_Count];
};

class Population : public Resources
{
};

class Storage : public Resources
{
};

class Team
{
public:

	Team(const std::string player, Race race, Colour colour);
	~Team();

	const std::string& GetPlayer() const { return m_player; }
	Race GetRace() const { return m_race; }
	Colour GetColour() const { return m_colour; }
private:
	std::string m_player;
	Race m_race;
	Colour m_colour;

	Population m_population;
	Storage m_storage;
	TechTrack m_tech;

	//ship blueprints
	//influence track
	//colony ships
	//rep points
};

typedef std::unique_ptr<Team> TeamPtr;

