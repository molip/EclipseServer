#pragma once

class InfluenceTrack
{
public:
	InfluenceTrack() : m_nDiscs(13) {} 

	int GetDiscCount() const { return m_nDiscs; }
	int GetUpkeep() const { return GetUpkeep(m_nDiscs); }

	void AddDiscs(int nDiscs) { m_nDiscs += nDiscs; }
	void RemoveDiscs(int nDiscs) { m_nDiscs -= nDiscs; }

	static int GetUpkeep(int nDiscs);

private:
	int m_nDiscs;
};
