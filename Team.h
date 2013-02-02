#pragma once

#include "Technology.h"
#include "TechTrack.h"

#include <vector>

enum class Resource { Money, Science, Materials, _Count };

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
	Team();
	~Team();

private:
	Population m_population;
	Storage m_storage;
	TechTrack m_tech;

	//ship blueprints
	//influence track
	//colony ships
	//rep points
};

