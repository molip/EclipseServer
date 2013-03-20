#include "stdafx.h"
#include "PopulationTrack.h"
#include "App.h"

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
