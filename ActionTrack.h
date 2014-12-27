#pragma once

#include "DiscTrack.h"

class ActionTrack : public DiscTrack
{
public:
	bool operator==(const ActionTrack& rhs) const { return __super::operator==(rhs); }
};
