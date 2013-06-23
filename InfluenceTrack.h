#pragma once

namespace Serial { class SaveNode; class LoadNode; }

class InfluenceTrack
{
public:
	InfluenceTrack() : m_nDiscs(0) {} 

	int GetDiscCount() const { return m_nDiscs; }
	int GetUpkeep() const { return GetUpkeep(m_nDiscs); }

	void AddDiscs(int nDiscs);
	void RemoveDiscs(int nDiscs);

	static int GetUpkeep(int nDiscs);

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	int m_nDiscs;
};
