#include "stdafx.h"
#include "TechTrack.h"
#include "App.h"
#include "Serial.h"

TechTrack::TechTrack()
{
}

bool TechTrack::CanAdd(TechType techtype) const
{
	if (Has(techtype))
		return false;

	Technology::Class c = Technology::GetClass(techtype);
	return GetCount(c) < 7;
}

void TechTrack::Add(TechType tech)
{
	AssertThrowModel("TechTrack::Add", CanAdd(tech));

	m_class[(int)Technology::GetClass(tech)].push_back(tech);
}

bool TechTrack::Has(TechType tech) const
{
	if (tech == TechType::None)
		return false;
	
	auto& c = m_class[(int)Technology::GetClass(tech)];
	return std::find(c.begin(), c.end(), tech) != c.end();
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

void TechTrack::Save(Serial::SaveNode& node) const
{
	for (auto c : EnumRange<Technology::Class>())
		node.SaveCntr(EnumTraits<Technology::Class>::ToString(c), m_class[(int)c], Serial::EnumSaver());
}

void TechTrack::Load(const Serial::LoadNode& node)
{
	for (auto c : EnumRange<Technology::Class>())
		node.LoadCntr(EnumTraits<Technology::Class>::ToString(c), m_class[(int)c], Serial::EnumLoader());
}
