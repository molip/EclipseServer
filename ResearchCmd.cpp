#include "stdafx.h"
#include "ResearchCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"
#include "CommitSession.h"

class ResearchRecord : public TeamRecord
{
public:
	ResearchRecord() : m_tech(TechType::None) {}
	ResearchRecord(Colour colour, TechType t) : TeamRecord(colour), m_tech(t) {}

	virtual void Save(Serial::SaveNode& node) const override 
	{
		__super::Save(node);
		node.SaveEnum("tech", m_tech);
	}
	
	virtual void Load(const Serial::LoadNode& node) override 
	{
		__super::Load(node);
		node.LoadEnum("tech", m_tech);
	}

private:
	virtual void Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState) override
	{
		auto& techs = gameState.GetTechnologies();
		auto it = techs.find(m_tech);
		
		// Add/remove tech from supply board.
		if (bDo)
		{
			VERIFY_MODEL(it != techs.end());
			if (--it->second == 0)
				techs.erase(it);
		}
		else		
		{
			if (it == techs.end())
				it = techs.insert(std::make_pair(m_tech, 0)).first;
			++it->second;
		}

		// Add/remove tech and science cost from team.
		int nCost = 0;
		if (bDo)
		{
			nCost = team.GetTechTrack().GetCost(m_tech);
			teamState.GetTechTrack().Add(m_tech);
		}
		else
		{
			teamState.GetTechTrack().Remove(m_tech);
			nCost = -team.GetTechTrack().GetCost(m_tech);
		}
		teamState.GetStorage()[Resource::Science] -= nCost;

		// Add/remove influence discs.
		if (int nDiscs = GetInfluenceDiscs())
			teamState.GetInfluenceTrack().AddDiscs(bDo ? nDiscs : -nDiscs);
	}

	virtual void Update(const Game& game, const Team& team, const RecordContext& context) const override
	{
		context.SendMessage(Output::UpdateTechnologies(game));
		context.SendMessage(Output::UpdateTechnologyTrack(team));
		context.SendMessage(Output::UpdateStorageTrack(team));
	
		if (int nDiscs = GetInfluenceDiscs())
			context.SendMessage(Output::UpdateInfluenceTrack(team));
	}

	virtual std::string GetTeamMessage() const
	{
		return FormatString("researched %0", ::EnumToString(m_tech));
	}

	int GetInfluenceDiscs() const
	{
		return m_tech == TechType::QuantumGrid ? 2 : m_tech == TechType::AdvRobotics ? 1 : 0;
	}

	TechType m_tech;
};

REGISTER_DYNAMIC(ResearchRecord)

//-----------------------------------------------------------------------------

ResearchCmd::ResearchCmd(Colour colour, const LiveGame& game, int iPhase) : PhaseCmd(colour, iPhase)
{
	const Team& team = GetTeam(game);
	VERIFY_INPUT(!team.HasPassed());
}

void ResearchCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	m_techs.clear();
	
	const Team& team = GetTeam(game);
	const int nScience = team.GetStorage()[Resource::Science];
	
	for (auto it : game.GetTechnologies())
	{
		TechType t = it.first;
		if (team.GetTechTrack().CanAdd(t))
		{
			int nCost = team.GetTechTrack().GetCost(t);
			if (nCost <= nScience)
				m_techs.push_back(std::make_pair(t, nCost));
		}
	}

	controller.SendMessage(Output::ChooseResearch(m_techs, m_iPhase > 0), GetPlayer(game));
}

CmdPtr ResearchCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	if (dynamic_cast<const Input::CmdAbort*>(&msg))
	{
		VERIFY_INPUT(m_iPhase > 0);
		return nullptr;
	}

	auto& m = VerifyCastInput<const Input::CmdResearch>(msg);
	VERIFY_INPUT_MSG("invalid tech index", InRange(m_techs, m.m_iTech));

	ResearchRecord* pRec = new ResearchRecord(m_colour, m_techs[m.m_iTech].first);
	DoRecord(RecordPtr(pRec), session);
	
	const LiveGame& game = session.GetGame();

	if (m_techs[m.m_iTech].first == TechType::ArtifactKey)
		return CmdPtr(new ResearchArtifactCmd(m_colour, game, m_iPhase));

	if (m_iPhase + 1 < Race(GetTeam(game).GetRace()).GetResearchRate())
		return CmdPtr(new ResearchCmd(m_colour, game, m_iPhase + 1));

	return nullptr;
}

void ResearchCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
}

void ResearchCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
}

REGISTER_DYNAMIC(ResearchCmd)

//-----------------------------------------------------------------------------

class ResearchArtifactRecord : public TeamRecord
{
public:
	ResearchArtifactRecord() {}
	ResearchArtifactRecord(Colour colour, const Storage& artifacts) : TeamRecord(colour), m_artifacts(artifacts) {}

	virtual void Save(Serial::SaveNode& node) const override 
	{
		__super::Save(node);
		node.SaveClass("artifacts", m_artifacts);
	}
	
	virtual void Load(const Serial::LoadNode& node) override 
	{
		__super::Load(node);
		node.LoadClass("artifacts", m_artifacts);
	}

private:
	virtual void Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState) override
	{
		for (auto r : EnumRange<Resource>())
			teamState.GetStorage()[r] += m_artifacts[r] * (bDo ? 5 : -5);
	}

	virtual void Update(const Game& game, const Team& team, const RecordContext& context) const override
	{
		context.SendMessage(Output::UpdateStorageTrack(team));
	}

	virtual std::string GetTeamMessage() const
	{
		return FormatString("used the artifact key");
	}

	Storage m_artifacts;
};

REGISTER_DYNAMIC(ResearchArtifactRecord)

//-----------------------------------------------------------------------------

ResearchArtifactCmd::ResearchArtifactCmd(Colour colour, const LiveGame& game, int iPhase) : PhaseCmd(colour, iPhase), m_nArtifacts(0)
{
}

void ResearchArtifactCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	m_nArtifacts = 0;

	for (auto& h : game.GetMap().GetHexes())
		if (h.second->IsOwnedBy(GetTeam(game)))
			m_nArtifacts += h.second->HasArtifact();

	controller.SendMessage(Output::ChooseResearchArtifact(m_nArtifacts), GetPlayer(game));
}

CmdPtr ResearchArtifactCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdResearchArtifact>(msg);

	ResearchArtifactRecord* pRec = new ResearchArtifactRecord(m_colour, m.m_artifacts);
	DoRecord(RecordPtr(pRec), session);

	const LiveGame& game = session.GetGame();
	if (m_iPhase + 1 < Race(GetTeam(game).GetRace()).GetResearchRate())
		return CmdPtr(new ResearchCmd(m_colour, game, m_iPhase + 1));

	return nullptr;
}

void ResearchArtifactCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
}

void ResearchArtifactCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
}

REGISTER_DYNAMIC(ResearchArtifactCmd)
