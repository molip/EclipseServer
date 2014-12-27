#pragma once

#include "EnumRange.h"
#include "EnumArray.h"

#include <map>
#include <vector>
#include <set>
#include <mutex>
#include <string>
#include <sstream>
#include <iostream>
#include <memory>
#include <algorithm>
#include <random>

//#define LOCK(m) ; //std::lock_guard<std::mutex> lock(m);
#define LOCK(m) std::lock_guard<std::mutex> lock(m);

#include "Debug.h"
#include "FormatString.h"

extern std::string FormatInt(int n);

extern int Mod(int a, int b);

template<typename T>
bool InRange(const T& cntr, int n)
{
	return n >= 0 && n < (int)cntr.size();
}

template <typename T>
class ReverseAdapter
{
public:
	ReverseAdapter(T t) : m_t(t) {}

	typename T::reverse_iterator begin() const { return m_t.rbegin(); }
	typename T::reverse_iterator end() const { return m_t.rend(); }
private:
	T m_t;
};

template <typename T>
ReverseAdapter<T> Reverse(const T& t) { return ReverseAdapter<T>(t); }

template <typename T>
bool ArePtrMapsEqual(const T& lhs, const T& rhs)
{
	if (lhs.size() == rhs.size())
		for (auto i = lhs.begin(), j = rhs.begin(); i != lhs.end(); ++i, ++j)
			if (i->first != j->first || !(*i->second == *j->second))
				return false;
	return true;
}


enum class SquareType;
enum class Resource;

extern Resource SquareTypeToResource(SquareType t);
extern SquareType ResourceToSquareType(Resource t);

extern std::default_random_engine& GetRandom();

#define DEFINE_UNIQUE_PTR(T) typedef std::unique_ptr<T> T##Ptr;

