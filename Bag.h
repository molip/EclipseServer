#pragma once

#include "Technology.h"
#include "Discovery.h"
#include "App.h"
#include "Serial.h"

template <typename T> 
class Bag 
{
public:
	bool IsEmpty() const { return m_vec.empty(); }
	
	T TakeTile()
	{
		VerifyModel("Bag::GetTile", !m_vec.empty());
		T t = m_vec.back();
		m_vec.pop_back();
		return t;
	}

	void ReturnTile(T t)
	{
		m_vec.push_back(t);
	}
protected:
	std::vector<T> m_vec;
};

template <typename T> 
class EnumBag : public Bag<T>
{
public:
	void Save(Serial::SaveNode& node) const
	{
		node.SaveCntr("tiles", m_vec, Serial::EnumSaver());
	}

	void Load(const Serial::LoadNode& node)
	{
		node.LoadCntr("tiles", m_vec, Serial::EnumLoader());
	}
};

class IntBag : public Bag<int>
{
public:
	void Save(Serial::SaveNode& node) const
	{
		node.SaveCntr("tiles", m_vec, Serial::TypeSaver());
	}

	void Load(const Serial::LoadNode& node)
	{
		node.LoadCntr("tiles", m_vec, Serial::TypeLoader());
	}
};

class DiscoveryBag : public EnumBag<DiscoveryType>
{
public:
	void Init();
};

class ReputationBag : public IntBag
{
public:
	void Init();
};

class TechnologyBag : public EnumBag<TechType>
{
public:
	void Init();
};

enum class HexRing; 
class HexBag : public IntBag
{
public:
	HexBag() {}
	HexBag(HexRing r, int nPlayers);
};
