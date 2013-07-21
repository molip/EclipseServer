#pragma once

#include "PhaseCmd.h"

#include <vector>

enum class TechType;

class ResearchCmd : public PhaseCmd
{
public:
	ResearchCmd() : m_bAborted(false) {}
	ResearchCmd(Colour colour, LiveGame& game, int iPhase = 0);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game) override;
	virtual void Undo(const Controller& controller, LiveGame& game) override;
	virtual bool IsAction() const { return m_iPhase == 0; } 
	virtual bool HasRecord() const { return !m_bAborted; } 

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	mutable std::vector<std::pair<TechType, int>> m_techs;
	bool m_bAborted;
};

class ResearchArtifactCmd : public PhaseCmd
{
public:
	ResearchArtifactCmd() : m_nArtifacts(0) {}
	ResearchArtifactCmd(Colour colour, LiveGame& game, int iPhase);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game) override;
	virtual void Undo(const Controller& controller, LiveGame& game) override;
	virtual bool HasRecord() const { return true; } 

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	mutable int m_nArtifacts;
};

