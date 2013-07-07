#pragma once

#include "Cmd.h"

class PhaseCmd : public Cmd
{
public:
	PhaseCmd() : m_iPhase(-1) {}
	PhaseCmd(Colour colour, int iPhase);

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

protected:
	int m_iPhase;
};
