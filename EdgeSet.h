#pragma once

#include "Resources.h"
#include "MapPos.h"

#include <vector>
#include <bitset>

extern Edge ReverseEdge(Edge e);
extern Edge RotateEdge(Edge e, int n);

class EdgeSet : public std::bitset<6>
{
public:
	EdgeSet() {}
	EdgeSet(std::string s);
	bool operator[](Edge e) const;
	EdgeSet::reference operator[](Edge e);
	std::vector<Edge> GetEdges() const;
};

