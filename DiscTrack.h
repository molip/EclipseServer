#pragma once

namespace Serial { class SaveNode; class LoadNode; }

class DiscTrack
{
public:
	DiscTrack() : m_nDiscs(0) {}

	int GetDiscCount() const { return m_nDiscs; }

	void AddDiscs(int nDiscs);
	void RemoveDiscs(int nDiscs);

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	int m_nDiscs;
};
