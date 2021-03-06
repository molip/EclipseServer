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
	ResearchRecord(Colour colour, TechType t, bool free) : TeamRecord(colour), m_tech(t), m_free(free) {}

	virtual void Save(Serial::SaveNode& node) const override 
	{
		__super::Save(node);
		node.SaveEnum("tech", m_tech);
		node.SaveType("free", m_free);
	}
	
	virtual void Load(const Serial::LoadNode& node) override 
	{
		__super::Load(node);
		node.LoadEnum("tech", m_tech);
		node.LoadType("free", m_free);
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

		if (!m_free)
		{
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

		}

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
	bool m_free;
};

REGISTER_DYNAMIC(ResearchRecord)

//-----------------------------------------------------------------------------

ResearchCmd::ResearchCmd(Colour colour, const LiveGame& game, int iPhase) : PhaseCmd(colour, iPhase)
{
	const Team& team = GetTeam(game);
	VERIFY_INPUT(!team.HasPassed());
}

ResearchCmd::TechVec ResearchCmd::GetTechs(const LiveGame& game) const
{
	TechVec techs;
	const Team& team = GetTeam(game);
	const int nScience = team.GetStorage()[Resource::Science];

	for (auto it : game.GetTechnologies())
	{
		TechType t = it.first;
		if (team.GetTechTrack().CanAdd(t))
		{
			int nCost = team.GetTechTrack().GetCost(t);
			if (nCost <= nScience)
				techs.push_back(std::make_pair(t, nCost));
		}
	}
	return techs;
}

void ResearchCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseResearch(GetTechs(game), m_iPhase > 0), GetPlayer(game));
}

Cmd::ProcessResult ResearchCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	if (dynamic_cast<const Input::CmdAbort*>(&msg))
	{
		VERIFY_INPUT(m_iPhase > 0);
		return nullptr;
	}

	const LiveGame& game = session.GetGame();
	ResearchCmd::TechVec techs = GetTechs(game);

	auto& m = VerifyCastInput<const Input::CmdResearch>(msg);
	VERIFY_INPUT_MSG("invalid tech index", InRange(techs, m.m_iTech));
	TechType tech = techs[m.m_iTech].first;
	bool free = techs[m.m_iTech].second == 0;

	ResearchRecord* pRec = new ResearchRecord(m_colour, tech, free);
	DoRecord(RecordPtr(pRec), session);
	
	if (tech == TechType::ArtifactKey)
		return ProcessResult(new ResearchArtifactCmd(m_colour, game, m_iPhase));

	if (CanResearchAgain(game))
		return ProcessResult(new ResearchCmd(m_colour, game, m_iPhase + 1));

	return nullptr;
}

bool ResearchCmd::CanResearchAgain(const LiveGame& game) const
{
	return m_iPhase + 1 < Race(GetTeam(game).GetRace()).GetResearchRate();
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

ResearchDiscoveryCmd::ResearchDiscoveryCmd(Colour colour, const LiveGame& game, std::vector<TechType> techs) : ResearchCmd(colour, game), m_techs(techs)
{
}

ResearchCmd::TechVec ResearchDiscoveryCmd::GetTechs(const LiveGame& game) const
{
	TechVec techs;
	for (auto&& tech : m_techs)
		techs.push_back(std::make_pair(tech, 0));
	return techs;
}

void ResearchDiscoveryCmd::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveCntr("techs", m_techs, Serial::EnumSaver());
}

void ResearchDiscoveryCmd::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadCntr("techs", m_techs, Serial::EnumLoader());
}

REGISTER_DYNAMIC(ResearchDiscoveryCmd)

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

Cmd::ProcessResult ResearchArtifactCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdResearchArtifact>(msg);

	ResearchArtifactRecord* pRec = new ResearchArtifactRecord(m_colour, m.m_artifacts);
	DoRecord(RecordPtr(pRec), session);

	const LiveGame& game = session.GetGame();
	if (m_iPhase + 1 < Race(GetTeam(game).GetRace()).GetResearchRate())
		return ProcessResult(new ResearchCmd(m_colour, game, m_iPhase + 1));

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
