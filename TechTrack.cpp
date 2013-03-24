#include "stdafx.h"
#include "TechTrack.h"
#include "App.h"

TechTrack::TechTrack()
{
}

bool TechTrack::CanAdd(TechType techtype) const
{
	if (Has(techtype))
		return false;

	Technology::Class c = Technology(techtype).GetClass();
	return GetCount(c) < 7;
}

void TechTrack::Add(TechType techtype)
{
	AssertThrowModel("TechTrack::Add", CanAdd(techtype));

	Technology t(techtype);
	m_class[(int)t.GetClass()].push_back(t);
	m_techs.insert(techtype);
}

bool TechTrack::Has(TechType tech) const
{
	return tech == TechType::None || m_techs.find(tech) != m_techs.end();
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
