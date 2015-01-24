#pragma once

#include "Resources.h"

class PopulationTrack
{
public:
	PopulationTrack();
	bool operator==(const PopulationTrack& rhs) const;

	int GetCount(Resource r) const { return m_pop[r]; }
	int GetIncome(Resource r) const { return GetIncome(m_pop[r]); }
	Storage GetIncome() const;

	void Add(Resource r, int nAdd);
	void Remove(Resource r, int nRemove) { Add(r, -nRemove); }

	const Population& GetPopulation() const { return m_pop; }
	Population GetEmptySpaces() const;

	static int GetIncome(int nPop);

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	Population m_pop;
};
