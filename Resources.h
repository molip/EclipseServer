#pragma once

#include <vector>

enum class Resource { None = -1, Money, Science, Materials, _Count };

struct OrbitalResourcesRange : EnumRange<Resource>
{
	OrbitalResourcesRange() : EnumRange<Resource>(Resource::Money, Resource::Materials) {}
};

class Resources : public EnumIntArray<Resource>
{
public:
	void operator+=(const Resources& rhs);
	void operator-=(const Resources& rhs);

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

	static bool IsOrbitalType(Resource r);

protected:
	Resources() {}
	Resources(int money, int sci, int mat) : EnumIntArray<Resource>({ money, sci, mat }) {}
};

class Population : public Resources
{
public:
	Population() {}
	Population(int money, int sci, int mat) : Resources(money, sci, mat) {}
	bool operator==(const Population& rhs) const { return std::operator==(*this, rhs); }
};

class Storage : public Resources
{
public:
	Storage() {}
	Storage(int money, int sci, int mat) : Resources(money, sci, mat) {}
	bool operator==(const Storage& rhs) const { return std::operator==(*this, rhs); }
};
