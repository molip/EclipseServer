#pragma once

#include "Technology.h"
#include "Discovery.h"
#include "App.h"
#include "Types.h"

template <typename T> 
class Bag 
{
public:
	typedef T TileType;
	const std::vector<T>& GetTiles() const { return m_vec; }
	std::vector<T>& GetTiles() { return m_vec; }

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

class TechnologyBag : public EnumBag<TechType>
{
public:
	void Init();
};

enum class HexRing; 
class HexPile : public IntBag
{
public:
	HexPile() {}
	
	static HexPile Create(HexRing r, int nPlayers);

	int TakeTile();
	void ReturnTile(int tile);

	bool IsEmpty() const { return m_vec.empty(); }
};

template <typename BagType>
class BagState
{
public:
	BagState() : m_bag(nullptr), m_taken(0) {}
	BagState(const BagState<BagType>& rhs) : m_bag(nullptr), m_taken(rhs.m_taken) {}

	void SetBag(const BagType& bag) { m_bag = &bag; }

	bool IsEmpty() const { return m_taken == m_bag->GetTiles().size(); }
	int GetRemaining() const { return int(m_bag->GetTiles().size() - m_taken); }
	int GetTaken() const { return (int)m_taken; }

	typename BagType::TileType TakeTile()
	{
		VERIFY_MODEL(!IsEmpty());
		return m_bag->GetTiles()[m_taken++];
	}

	typename BagType::TileType ReturnTile()
	{
		VERIFY_MODEL(m_taken > 0);
		return m_bag->GetTiles()[--m_taken];
	}

	void Save(Serial::SaveNode& node) const
	{
		node.SaveType("taken", m_taken);
	}

	void Load(const Serial::LoadNode& node)
	{
		node.LoadType("taken", m_taken);
	}

private:
	const BagType* m_bag;
	size_t m_taken;
};

class ReputationBag
{
public:
	ReputationBag();

	bool IsEmpty() const { return GetTileCount() == 0; }
	int GetTileCount() const;
	int ChooseBestTile(int count) const;

	int ChooseAndTakeTile();
	void TakeTile(int val);
	void ReturnTile(int val);

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	std::array<int, 4> m_counts;
};
