#include "stdafx.h"
#include "InfluenceTrack.h"
#include "App.h"
#include "Serial.h"

#include <algorithm>

void InfluenceTrack::AddDiscs(int nDiscs) 
{
	int n = m_nDiscs + nDiscs;
	AssertThrowModel("InfluenceTrack::AddDiscs", n >= 0 && n <= 16);
	m_nDiscs = n; 
}

void InfluenceTrack::RemoveDiscs(int nDiscs) 
{
	AddDiscs(-nDiscs); 
}

int InfluenceTrack::GetUpkeep(int nDiscs)
{
	static const int vals[] = { 30, 25, 21, 17, 13, 10, 7, 5, 3, 2, 1, 0, 0, 0, 0, 0 };

	AssertThrowModel("InfluenceTrack::GetUpkeep", nDiscs >= 0 && nDiscs < _countof(vals));
	return vals[nDiscs];
}

void InfluenceTrack::Save(Serial::SaveNode& node) const
{
	node.SaveType("discs", m_nDiscs);
}

void InfluenceTrack::Load(const Serial::LoadNode& node)
{
	node.LoadType("discs", m_nDiscs);
}