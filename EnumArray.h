#pragma once

#include <array>

template <typename E, typename T>
class EnumArray : public std::array<T, (int)E::_Count>
{
public:
	EnumArray() : std::array<T, (int)E::_Count>({}) {}

	const T& operator[] (E e) const { return at((int)e); }
	T& operator[] (E e) { return at((int)e); }

	const T& operator[] (int i) const { return at(i); }
	T& operator[] (int i) { return at(i); }
};
