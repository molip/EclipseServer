#include "stdafx.h"
#include "TechTrack.h"

int TechTrack::GetNextDiscount(Technology::Class c) const
{
	return GetDiscount(m_class[(int)c].size());
}

int TechTrack::GetDiscount(int tier)
{
	return tier < 5 ? tier : 6 + (tier - 5) * 2;
}
