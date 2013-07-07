#pragma once

#include "Technology.h"

#include <vector>
#include <set>

namespace Serial { class SaveNode; class LoadNode; }

class TechTrack
{
public:
	TechTrack();

	void Add(TechType tech);
	void Remove(TechType tech);
	bool Has(TechType tech) const;
	bool CanAdd(TechType techtype) const;

	int GetNextDiscount(Technology::Class c) const;
	static int GetDiscount(int tier);
	int GetCount(Technology::Class c) const;
	int GetCost(TechType t) const;

	const std::vector<TechType>& GetClass(Technology::Class c) const { return m_class[(int)c]; }

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	std::vector<TechType> m_class[3];
};
