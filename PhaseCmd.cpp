#include "stdafx.h"
#include "PhaseCmd.h"

PhaseCmd::PhaseCmd(Colour colour, int iPhase) : Cmd(colour), m_iPhase(iPhase)
{
}

void PhaseCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveType("phase", m_iPhase);
}

void PhaseCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadType("phase", m_iPhase);
}

