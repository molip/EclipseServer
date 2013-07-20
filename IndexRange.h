#pragma once

template <typename T> class IndexIter
{
public:
    IndexIter(std::function<T(int)> fnGet, int i) : m_index(i), m_fnGet(fnGet) {}
	bool operator !=(const IndexIter& rhs) const { return m_index != rhs.m_index; }
	T operator* () const { return m_fnGet(m_index); }
    void operator++ () { ++m_index; }
private:
    int m_index;
	std::function<T(int)> m_fnGet;
};
 
template <typename T> class IndexRange
{
public:
	IndexRange(std::function<T(int)> fnGet, int count) : m_fnGet(fnGet), m_count(count) {}
	IndexIter<T> begin() const { return IndexIter<T>(m_fnGet, 0); }
	IndexIter<T> end() const { return IndexIter<T>(m_fnGet, m_count); }
private:
	std::function<T(int)> m_fnGet;
	int m_count;
};

#define DEFINE_INDEXRANGE(rangename, classname, type, getTypeFn, getCountFn) \
class rangename : public IndexRange<type>\
{\
public:\
	rangename(const classname& obj) : IndexRange( [&](int i) { return obj.getTypeFn(i); }, obj.getCountFn()) {}\
};
