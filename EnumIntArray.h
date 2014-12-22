#pragma once

#include <array>

template <typename T>
class EnumIntArray : public std::array<int, (int)T::_Count>
{
public:
	EnumIntArray() : std::array<int, (int)T::_Count>({}) {}

	const int& operator[] (T t) const { return at((int)t); }
	int& operator[] (T t) { return at((int)t); }

	const int& operator[] (int i) const { return at(i); }
	int& operator[] (int i) { return at(i); }
};
