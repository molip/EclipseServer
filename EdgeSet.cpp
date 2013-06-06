#include "stdafx.h"
#include "EdgeSet.h"
#include "App.h"

Edge ReverseEdge(Edge e)
{
	return RotateEdge(e, 3);
}

Edge RotateEdge(Edge e, int n)
{
	return (Edge)Mod(int(e) + n, 6);
}

//-----------------------------------------------------------------------------

EdgeSet::EdgeSet(std::string s)
{
	std::reverse(s.begin(), s.end());
	__super::operator =(std::bitset<6>(s));
}

bool EdgeSet::operator[](Edge e) const
{
	return __super::operator[]((int)e);
}

EdgeSet::reference EdgeSet::operator[](Edge e)
{
	return __super::operator[]((int)e);
}

std::vector<Edge> EdgeSet::GetEdges() const
{
	std::vector<Edge> edges;
	for (int i = 0; i < 6; ++i)
		if (__super::operator[](i))
			edges.push_back((Edge)i);
	return edges;
}

