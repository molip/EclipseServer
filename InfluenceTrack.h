#pragma once

#include "DiscTrack.h"

class InfluenceTrack : public DiscTrack
{
public:
	bool operator==(const InfluenceTrack& rhs) const { return __super::operator==(rhs); }
	int GetUpkeep() const { return GetUpkeep(GetDiscCount()); }
	static int GetUpkeep(int nDiscs);
};
