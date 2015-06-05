#include "stdafx.h"
#include "DiscTrack.h"

#include "App.h"

bool DiscTrack::operator==(const DiscTrack& rhs) const
{
	return m_nDiscs == rhs.m_nDiscs;
}

void DiscTrack::AddDiscs(int nDiscs)
{
	int n = m_nDiscs + nDiscs;
	VERIFY_MODEL(n >= 0 && n <= 16);
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