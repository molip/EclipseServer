#pragma once

#include "Debug.h"

#include <array>

template <typename E, typename T>
class EnumArray : public std::array<T, (int)E::_Count>
{
	typedef std::array<T, (int)E::_Count> Base;
public:
	EnumArray() : Base({}) {}
	EnumArray(std::initializer_list<T> i) 
	{
		VERIFY(i.size() == (int)E::_Count); 
		std::copy(i.begin(), i.end(), begin());
	}

	const T& at(E e) const { return __super::at((int)e); }
	T& at(E e) { return __super::at((int)e); }

	const T& operator[] (E e) const { return at(e); }
	T& operator[] (E e) { return at(e); }

	const T& operator[] (int i) const { return Base::at(i); }
	T& operator[] (int i) { return Base::at(i); }
};

template <typename E>
class EnumIntArray : public EnumArray<E, int>
{
public:
	EnumIntArray() {}
	EnumIntArray(std::initializer_list<int> i) : EnumArray<E, int>(i) {}

	int GetTotal() const { int n = 0; for (auto i : *this) n += i; return n; }
	bool IsEmpty() const { for (auto i : *this) if (i) return false; return true; }

};
