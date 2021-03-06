#include "stdafx.h"
#include "TechTrack.h"
#include "App.h"

TechTrack::TechTrack()
{
}

bool TechTrack::operator==(const TechTrack& rhs) const
{
	return m_classes == rhs.m_classes;
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
	VERIFY_MODEL(CanAdd(tech));

	m_classes[Technology::GetClass(tech)].push_back(tech);
}

void TechTrack::Remove(TechType tech)
{
	VERIFY_MODEL(Has(tech));

	auto& c = m_classes[Technology::GetClass(tech)];
	c.erase(std::remove(c.begin(), c.end(), tech), c.end());
}

bool TechTrack::Has(TechType tech) const
{
	VERIFY_MODEL(tech != TechType::None);
	
	auto& c = m_classes[Technology::GetClass(tech)];
	return std::find(c.begin(), c.end(), tech) != c.end();
}

int TechTrack::GetCount(Technology::Class c) const
{
	return (int)m_classes[c].size();
}

int TechTrack::GetNextDiscount(Technology::Class c) const
{
	return GetDiscount(GetCount(c));
}

int TechTrack::GetDiscount(int tier)
{
	return tier < 5 ? tier : 6 + (tier - 5) * 2;
}

int TechTrack::GetVictoryPoints(int tier)
{
	return tier < 3 ? 0 : tier < 6 ? tier - 3 : 5;
}

int TechTrack::GetCost(TechType t) const
{
	Technology::Class c = Technology::GetClass(t);
	return std::max(Technology::GetMinCost(t), Technology::GetMaxCost(t) - GetNextDiscount(c));
}

int TechTrack::GetVictoryPoints() const
{
	int score = 0;
	for (auto c : EnumRange<Technology::Class>())
		score += GetVictoryPoints(GetCount(c));
	return 0;
}

void TechTrack::Save(Serial::SaveNode& node) const
{
	for (auto c : EnumRange<Technology::Class>())
		node.SaveCntr(::EnumToString(c), m_classes[c], Serial::EnumSaver());
}

void TechTrack::Load(const Serial::LoadNode& node)
{
	for (auto c : EnumRange<Technology::Class>())
		node.LoadCntr(::EnumToString(c), m_classes[c], Serial::EnumLoader());
}
