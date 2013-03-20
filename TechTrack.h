#pragma once

#include "Technology.h"

#include <vector>

class TechTrack
{
public:
	TechTrack();

	bool Add(TechType tech);

	int GetNextDiscount(Technology::Class c) const;
	static int GetDiscount(int tier);
	int GetCount(Technology::Class c) const;

private:
	std::vector<Technology> m_class[3];
};
