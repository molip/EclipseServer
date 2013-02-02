#pragma once

#include "Technology.h"

#include <vector>

class TechTrack
{
public:
	TechTrack() {} 

	int GetNextDiscount(Technology::Class c) const;

	static int GetDiscount(int tier);

private:
	std::vector<Technology> m_class[3];
};
