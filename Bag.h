#pragma once

#include "Technology.h"
#include "Discovery.h"
#include "App.h"

template <typename T> 
class Bag : protected std::vector<T>
{
public:
	bool IsEmpty() const { return empty(); }
	
	T TakeTile()
	{
		AssertThrowModel("Bag::GetTile", !empty());
		T t = back();
		pop_back();
		return t;
	}
};

class DiscoveryBag : public Bag<DiscoveryType>
{
public:
	DiscoveryBag();
};

class ReputationBag : public Bag<int>
{
public:
	ReputationBag();
};

class TechnologyBag : public Bag<TechType>
{
public:
	TechnologyBag();
};

