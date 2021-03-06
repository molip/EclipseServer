#pragma once

#include "PhaseCmd.h"

#include <vector>

enum class TechType;

class ResearchCmd : public PhaseCmd
{
public:
	ResearchCmd() {}
	ResearchCmd(Colour colour, const LiveGame& game, int iPhase = 0);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;
	virtual bool IsAction() const { return m_iPhase == 0; } 
	virtual std::string GetActionName() const override { return "Research"; }

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

protected:
	typedef std::vector<std::pair<TechType, int>> TechVec;
	virtual TechVec GetTechs(const LiveGame& game) const;
	virtual bool CanResearchAgain(const LiveGame& game) const;
};

class ResearchDiscoveryCmd : public ResearchCmd
{
public:
	ResearchDiscoveryCmd() {}
	ResearchDiscoveryCmd(Colour colour, const LiveGame& game, std::vector<TechType> techs);

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	virtual TechVec GetTechs(const LiveGame& game) const override;
	virtual bool CanResearchAgain(const LiveGame& game) const override { return false; }
	std::vector<TechType> m_techs;
};

class ResearchArtifactCmd : public PhaseCmd
{
public:
	ResearchArtifactCmd() : m_nArtifacts(0) {}
	ResearchArtifactCmd(Colour colour, const LiveGame& game, int iPhase);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	mutable int m_nArtifacts;
};

