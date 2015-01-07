#include "stdafx.h"
#include "DiscoverCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "Player.h"
#include "Race.h"
#include "Team.h"
#include "LiveGame.h"
#include "ActionPhase.h"
#include "CommitSession.h"
#include "Record.h"
#include "UpgradeCmd.h"
#include "ResearchCmd.h"
#include "Technology.h"
#include "TechTrack.h"

class DiscoveryRecord : public TeamRecord
{
public:
	DiscoveryRecord() : m_discovery(DiscoveryType::None), m_idHex(0) {}
	DiscoveryRecord(Colour colour, DiscoveryType discovery, int idHex) : TeamRecord(colour), m_discovery(discovery), m_idHex(idHex) {}

private:
	virtual void Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState) override
	{
		switch (Discovery::GetClass(m_discovery))
		{
		case DiscoveryClass::None: // Victory points.
			teamState.m_victoryPointTiles += bDo ? 1 : -1;
			break;
		case DiscoveryClass::Resource: // Add to storage.
			if (bDo)
				teamState.GetStorage() += Discovery::GetResources(m_discovery);
			else
				teamState.GetStorage() -= Discovery::GetResources(m_discovery);
			break;
		case DiscoveryClass::ShipPart: 
		{
			ShipPart part = Discovery::GetShipPart(m_discovery);
			if (bDo)
				VERIFY(teamState.m_discoveredShipParts.insert(part).second);
			else
				VERIFY(teamState.m_discoveredShipParts.erase(part) == 1);
			break;
		}
		case DiscoveryClass::Ship: // Add to hex. 
		{
			Hex* hex = gameState.GetMap().FindHex(m_idHex);
			VERIFY(!!hex);
			if (bDo)
				hex->AddShip(ShipType::Cruiser, m_colour);
			else
				hex->RemoveShip(ShipType::Cruiser, m_colour);
				
			teamState.AddShips(ShipType::Cruiser, bDo ? -1 : 1);
			break;
		}
		default: 
			VERIFY(false); // Tech - should have started UpgradeCmd. 
		}
	}

	virtual void Update(const Game& game, const Team& team, const RecordContext& context) const override
	{
		switch (Discovery::GetClass(m_discovery))
		{
		case DiscoveryClass::None: 
			context.SendMessage(Output::UpdateVictoryPointTiles(team));
			break;
		case DiscoveryClass::Resource: 
			context.SendMessage(Output::UpdateStorageTrack(team));
			break;
		case DiscoveryClass::ShipPart: 
			break;
		case DiscoveryClass::Ship: 
			context.SendMessage(Output::UpdateMap(game));
			break;
		};
	}

	virtual std::string GetTeamMessage() const
	{
		std::string msg = "discovered ";

		if (m_discovery == DiscoveryType::None)
			msg += "2 victory points";
		else
		{
			if (Discovery::GetClass(m_discovery) == DiscoveryClass::Resource)
			{
				Storage s = Discovery::GetResources(m_discovery);
				for (auto r : EnumRange<Resource>())
					if (s[r])
					{
						msg += ::FormatString("%0 ", s[r]);
						break;
					}
			}
			msg += ::EnumToString(m_discovery);
		}
		return msg;
	}

	virtual void Save(Serial::SaveNode& node) const override
	{
		__super::Save(node);
		node.SaveEnum("discovery", m_discovery);
		node.SaveType("hex_id", m_idHex);
	}

	virtual void Load(const Serial::LoadNode& node) override
	{
		__super::Load(node);
		node.LoadEnum("discovery", m_discovery);
		node.LoadType("hex_id", m_idHex);
	}

	DiscoveryType m_discovery;
	int m_idHex;
};
REGISTER_DYNAMIC(DiscoveryRecord)

//-----------------------------------------------------------------------------

DiscoverCmd::DiscoverCmd(Colour colour, const LiveGame& game, DiscoveryType discovery, int idHex) :
Cmd(colour), m_discovery(discovery), m_idHex(idHex)
{
}

bool DiscoverCmd::CanKeep() const
{
	return Discovery::GetClass(m_discovery) == DiscoveryClass::ShipPart;
}

bool DiscoverCmd::CanUse(const LiveGame& game) const
{
	if (m_discovery == DiscoveryType::AncientCruiser)
		return GetTeam(game).GetUnusedShips(ShipType::Cruiser) > 0;
	if (m_discovery == DiscoveryType::AncientTech)
		return !GetAncientTechs(game).empty();
	return true;
}

std::vector<TechType> DiscoverCmd::GetAncientTechs(const LiveGame& game) const
{
	// "the cheapest Technology Tile you don't already have". Ignoring discounts. 

	// Get all available unresearched techs.
	std::multimap<int, TechType> map;
	auto& team = GetTeam(game);
	for (auto&& pair : game.GetTechnologies())
		if (!team.GetTechTrack().Has(pair.first))
			map.insert(std::make_pair(Technology::GetMaxCost(pair.first), pair.first));

	std::vector<TechType> techs;
	if (!map.empty())
	{
		auto it = map.begin();
		int cheapest = it->first;
		for (; it->first == cheapest; ++it)
			techs.push_back(it->second);
	}
	return techs;
}

void DiscoverCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseDiscovery(m_discovery, CanKeep(), CanUse(game)), GetPlayer(game));
}

Cmd::ProcessResult DiscoverCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdDiscovery>(msg);
	
	VERIFY_INPUT(m.m_action != Input::CmdDiscovery::Action::None);

	auto& game = session.GetGame();

	bool use = m.m_action == Input::CmdDiscovery::Action::Use;
	VERIFY_INPUT(!use || CanUse(game));

	if (use && m_discovery == DiscoveryType::AncientTech)
		return ProcessResult(new ResearchDiscoveryCmd(m_colour, game, GetAncientTechs(game)));

	DiscoveryType discovery = m.m_action == Input::CmdDiscovery::Action::Points ? DiscoveryType::None : m_discovery;
	DoRecord(RecordPtr(new DiscoveryRecord(m_colour, discovery, m_idHex)), session);

	if (use && Discovery::GetClass(m_discovery) == DiscoveryClass::ShipPart)
		return ProcessResult(new UpgradeDiscoveryCmd(m_colour, session.GetGame(), Discovery::GetShipPart(m_discovery)));

	return ProcessResult(nullptr);
}

void DiscoverCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
	node.SaveEnum("discovery", m_discovery);
	node.SaveType("hex_id", m_idHex);
}

void DiscoverCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
	node.LoadEnum("discovery", m_discovery);
	node.LoadType("hex_id", m_idHex);
}

REGISTER_DYNAMIC(DiscoverCmd)
