#pragma once

#include "DiscTrack.h"

class InfluenceTrack : public DiscTrack
{
public:
	int GetUpkeep() const { return GetUpkeep(GetDiscCount()); }
	static int GetUpkeep(int nDiscs);
};
