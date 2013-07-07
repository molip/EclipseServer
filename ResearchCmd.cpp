#include "stdafx.h"
#include "ResearchCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"

class ResearchRecord : public Record
{
public:
	ResearchRecord() : m_colour(Colour::None), m_tech(TechType::None) {}
	ResearchRecord(Colour colour, TechType t) : m_colour(colour), m_tech(t) {}

	virtual void Save(Serial::SaveNode& node) const override 
	{
		node.SaveEnum("colour", m_colour);
		node.SaveEnum("tech", m_tech);
	}
	
	virtual void Load(const Serial::LoadNode& node) override 
	{
		node.LoadEnum("colour", m_colour);
		node.LoadEnum("tech", m_tech);
	}

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) override
	{
		auto& techs = game.GetTechnologies();
		auto it = techs.find(m_tech);
		
		// Add/remove tech from supply board.
		if (bDo)
		{
			AssertThrow("ResearchRecord::Apply", it != techs.end());
			if (--it->second == 0)
				techs.erase(it);
		}
		else		
		{
			if (it == techs.end())
				it = techs.insert(std::make_pair(m_tech, 0)).first;
			++it->second;
		}

		Team& team = game.GetTeam(m_colour);
		
		// Add/remove tech and science cost from team.
		int nCost = 0;
		if (bDo)
		{
			nCost = team.GetTechTrack().GetCost(m_tech);
			team.GetTechTrack().Add(m_tech);
		}
		else
		{
			team.GetTechTrack().Remove(m_tech);
			nCost = -team.GetTechTrack().GetCost(m_tech);
		}
		team.GetStorage()[Resource::Science] -= nCost;

		// Add/remove influence discs.
		if (int nDiscs = m_tech == TechType::QuantumGrid ? 2 : m_tech == TechType::AdvRobotics ? 1 : 0)
		{
			team.GetInfluenceTrack().AddDiscs(bDo ? nDiscs : -nDiscs);
			controller.SendMessage(Output::UpdateInfluenceTrack(team), game);
		}

		controller.SendMessage(Output::UpdateTechnologies(game), game);
		controller.SendMessage(Output::UpdateTechnologyTrack(team), game);
		controller.SendMessage(Output::UpdateStorageTrack(team), game);
	}

	Colour m_colour;
	TechType m_tech;
};

REGISTER_DYNAMIC(ResearchRecord)

//-----------------------------------------------------------------------------

ResearchCmd::ResearchCmd(Colour colour, LiveGame& game) : Cmd(colour)
{
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

	controller.SendMessage(Output::ChooseResearch(m_techs), GetPlayer(game));
}

CmdPtr ResearchCmd::Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game)
{
	auto& m = CastThrow<const Input::CmdResearch>(msg);
	AssertThrow("ResearchCmd::Process: invalid tech index", InRange(m_techs, m.m_iTech));

	ResearchRecord* pRec = new ResearchRecord(m_colour, m_techs[m.m_iTech].first);
	pRec->Do(game, controller);
	game.PushRecord(RecordPtr(pRec));

	if (m_techs[m.m_iTech].first == TechType::ArtifactKey)
		AssertThrow("ResearchCmd::Process: ArtifactKey NYI", false); // TODO:

	return nullptr;
}

void ResearchCmd::Undo(const Controller& controller, LiveGame& game)
{
	RecordPtr pRec = game.PopRecord();
	pRec->Undo(game, controller);
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
