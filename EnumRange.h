#pragma once

template <typename T>
class EnumIter
{
public:
    EnumIter(T t) : m_t(t) {}
	bool operator !=(const EnumIter<T>& rhs) const { return m_t != rhs.m_t; }
	T operator* () const { return m_t; }
    void operator++ () { m_t = T(1 + (int)m_t); }
private:
    T m_t;
};
 
template <typename T>
class EnumRange
{
public:
	EnumRange(T begin = T(0), T end = T::_Count) : m_begin(begin), m_end(end) {}
	EnumIter<T> begin() const { return EnumIter<T>(m_begin); }
	EnumIter<T> end() const { return EnumIter<T>(m_end); }
private:
	T m_begin, m_end;
};

