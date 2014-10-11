#pragma once

#include <vector>
#include "EnumRange.h"

namespace Serial { class SaveNode; class LoadNode; }

enum class Resource { None = -1, Money, Science, Materials, _Count };

struct OrbitalResourcesRange : EnumRange<Resource>
{
	OrbitalResourcesRange() : EnumRange<Resource>(Resource::Money, Resource::Materials) {}
};

class Resources : private std::vector<int>
{
public:
	const int& operator[] (Resource r) const { return at(r); }
	int& operator[] (Resource r) { return at(r); }

	void operator+=(const Resources& rhs);
	void operator-=(const Resources& rhs);

	int GetTotal() const { int n = 0; for (auto& r : *this) n += r; return n; }
	bool IsEmpty() const { for (auto& r : *this) if (r) return false; return true; }

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

protected:
	Resources(int money = 0, int sci = 0, int mat = 0) 
	{
		// Same order as enum.
		push_back(money);
		push_back(sci);
		push_back(mat);
	}

	const int& at(Resource r) const { return __super::at((int)r); }
	int& at(Resource r) { return __super::at((int)r); }

private:
};

class Population : public Resources
{
public:
	Population() {}
	Population(int money, int sci, int mat) : Resources(money, sci, mat) {}
};

class Storage : public Resources
{
public:
	Storage() {}
	Storage(int money, int sci, int mat) : Resources(money, sci, mat) {}
};
