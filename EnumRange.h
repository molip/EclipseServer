#pragma once

#include <cassert>

template <typename T, bool reverse>
class EnumIter
{
public:
    EnumIter(T t) : m_i(int(t)) {}
	bool operator !=(const EnumIter<T, reverse>& rhs) const { return m_i != rhs.m_i; }
	T operator* () const { return (T)m_i; }
	void operator++ () { reverse ? --m_i : ++m_i; }
private:
    int m_i;
};

template <typename T>
class EnumRange
{
public:
	EnumRange(T begin = T(0), T end = T::_Count) : m_begin(begin), m_end(end) { assert(int(begin) < int(end));  }
	
	typedef EnumIter<T, true> reverse_iterator;
	typedef EnumIter<T, false> iterator;

	iterator begin() const { return iterator(m_begin); }
	iterator end() const { return iterator(m_end); }
	
	reverse_iterator rbegin() const { return reverse_iterator(T(int(m_begin) - 1)); }
	reverse_iterator rend() const { return reverse_iterator(T(int(m_end) - 1)); }
private:
	T m_begin, m_end;
};
