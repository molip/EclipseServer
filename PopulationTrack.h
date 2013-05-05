#pragma once

#include "Resources.h"

class PopulationTrack
{
public:
	PopulationTrack();

	int GetCount(Resource r) const { return m_pop[r]; }
	int GetIncome(Resource r) const { return GetIncome(m_pop[r]); }

	void Add(Resource r, int nAdd);
	void Remove(Resource r, int nRemove) { Add(r, -nRemove); }

	const Population& GetPopulation() const { return m_pop; }

	static int GetIncome(int nPop);

private:
	Population m_pop;
};
