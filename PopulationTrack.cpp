#include "stdafx.h"
#include "PopulationTrack.h"
#include "App.h"
#include "Serial.h"

#include <algorithm>

namespace 
{
	const int MaxPop = 12;
}

PopulationTrack::PopulationTrack() : m_pop(11, 11, 11)
{
}

void PopulationTrack::Add(Resource r, int nAdd)
{
	int nNew = m_pop[r] + nAdd;
	AssertThrowModel("PopulationTrack::Add", nNew >= 0 && nNew <= MaxPop);
	m_pop[r] = nNew;
}

int PopulationTrack::GetIncome(int nPop)
{
	static const int vals[] = { 28, 24, 21, 18, 15, 12, 10, 8, 6, 4, 3, 2 };
	AssertThrowModel("PopulationTrack::GetIncome", nPop >= 0 && nPop < _countof(vals));
	return vals[nPop];
}

Storage PopulationTrack::GetIncome() const
{
	Storage income;
	for (auto r : EnumRange<Resource>())
		income[r] = GetIncome(r);
	return income;
}

void PopulationTrack::Save(Serial::SaveNode& node) const
{
	node.SaveClass("pop", m_pop);
}

void PopulationTrack::Load(const Serial::LoadNode& node)
{
	node.LoadClass("pop", m_pop);
}
