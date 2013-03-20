#include "stdafx.h"
#include "TechTrack.h"

TechTrack::TechTrack()
{
}

bool TechTrack::Add(TechType techtype)
{
	Technology t(techtype);
	Technology::Class c = t.GetClass();
	if (GetCount(c) == 7)
		return false;

	m_class[(int)c].push_back(t);
	return true;
}

int TechTrack::GetCount(Technology::Class c) const
{
	return m_class[(int)c].size();
}

int TechTrack::GetNextDiscount(Technology::Class c) const
{
	return GetDiscount(GetCount(c));
}

int TechTrack::GetDiscount(int tier)
{
	return tier < 5 ? tier : 6 + (tier - 5) * 2;
}
