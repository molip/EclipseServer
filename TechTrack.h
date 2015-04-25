#pragma once

#include "Technology.h"
#include "EnumArray.h"

#include <vector>
#include <set>
#include <array>

namespace Serial { class SaveNode; class LoadNode; }

class TechTrack
{
public:
	TechTrack();
	bool operator==(const TechTrack& rhs) const;

	void Add(TechType tech);
	void Remove(TechType tech);
	bool Has(TechType tech) const;
	bool CanAdd(TechType techtype) const;

	int GetNextDiscount(Technology::Class c) const;
	static int GetDiscount(int tier);
	static int GetVictoryPoints(int tier);
	int GetCount(Technology::Class c) const;
	int GetCost(TechType t) const;
	int GetVictoryPoints() const;

	const std::vector<TechType>& GetClass(Technology::Class c) const { return m_classes[c]; }

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	EnumArray<Technology::Class, std::vector<TechType>> m_classes;
};
