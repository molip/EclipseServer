#include "stdafx.h"
#include "DiscTrack.h"
#include "Serial.h"

void DiscTrack::AddDiscs(int nDiscs)
{
	int n = m_nDiscs + nDiscs;
	AssertThrowModel("DiscTrack::AddDiscs", n >= 0 && n <= 16);
	m_nDiscs = n; 
}

void DiscTrack::RemoveDiscs(int nDiscs)
{
	AddDiscs(-nDiscs); 
}

void DiscTrack::Save(Serial::SaveNode& node) const
{
	node.SaveType("discs", m_nDiscs);
}

void DiscTrack::Load(const Serial::LoadNode& node)
{
	node.LoadType("discs", m_nDiscs);
}