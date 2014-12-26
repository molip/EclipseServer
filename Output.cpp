#include "Output.h"
#include "App.h"
#include "Team.h"
#include "Race.h"
#include "EnumRange.h"
#include "Player.h"
#include "Technology.h"
#include "EnumTraits.h"
#include "LiveGame.h"
#include "ReviewGame.h"
#include "Games.h"
#include "ActionPhase.h"
#include "Battle.h"
#include "CombatPhase.h"
#include "Dice.h"

namespace
{
	void AddNonOwnerPlayers(const Game& game, Json::Element& root)
	{
		auto playersNode = root.AddArray("players");
		for (auto& i : game.GetTeams())
			if (i->GetPlayerID() != game.GetOwner().GetID())
				playersNode.Append(i->GetPlayer().GetName());
	}
	void AddPointElement(int x, int y, const std::string& name, Json::Element& root)
	{
		auto node = root.AddElement(name);
		node.SetAttribute("x", x);
		node.SetAttribute("y", y);
	}
	void AppendPointElement(int x, int y, Json::Array& array)
	{
		auto node = array.AppendElement();
		node.SetAttribute("x", x);
		node.SetAttribute("y", y);
	}
}

namespace Output
{

std::string Message::GetXML() const
{
	return m_doc.SaveToString();
}

//--------------------------------------------

Response::Response()
{
	m_root = m_doc.AddElement("response");
}

Command::Command(const std::string& cmd) 
{
	m_root = m_doc.AddElement("command");
	m_root.SetAttribute("type", cmd);
}

Show::Show(const std::string& panel) : Command("show")
{
	m_root.SetAttribute("panel", panel);
}

Update::Update(const std::string& param) : Command("update")
{
	m_root.SetAttribute("param", param);
}

Choose::Choose(const std::string& param) : Command("choose")
{
	m_root.SetAttribute("param", param);
}

UpdateGameList::UpdateGameList() : Update("game_list")
{
	auto gamesNode = m_root.AddArray("games");
	for (auto& g : Games::GetLiveGames())
	{
		auto gameNode = gamesNode.AppendElement();
		gameNode.SetAttribute("name", g->GetName());
		gameNode.SetAttribute("id", g->GetID());
		gameNode.SetAttribute("owner", g->GetOwner().GetName());
		gameNode.SetAttribute("started", g->HasStarted());
	
		AddNonOwnerPlayers(*g, gameNode);
	}
}

UpdateLobby::UpdateLobby(const Game& game) : Update("lobby")
{
	m_root.SetAttribute("owner", game.GetOwner().GetName());
	m_root.SetAttribute("game", game.GetName());
	AddNonOwnerPlayers(game, m_root);
}

UpdateLobbyControls::UpdateLobbyControls(bool bShow) : Update("lobby_controls")
{
	m_root.SetAttribute("show", bShow);
}

UpdateChoose::UpdateChoose(const LiveGame& game) : Update("choose_team")
{
	m_root.SetAttribute("game", game.GetName());
	auto teamsNode = m_root.AddArray("teams");
	for (auto& t : game.GetTeams())
	{
		auto teamNode = teamsNode.AppendElement();
		teamNode.SetAttribute("name", t->GetPlayer().GetName());
		if (t->GetRace() != RaceType::None)
		{
			teamNode.SetAttribute("race", ::EnumToString(t->GetRace()));
			teamNode.SetAttribute("colour", ::EnumToString(t->GetColour()));
		}
	}
}

UpdateTeams::UpdateTeams(const Game& game) : Update("teams")
{
	VERIFY_MODEL_MSG("Game not started yet", game.HasStarted());

	m_root.SetAttribute("game_type", game.IsLive() ? "live" : "review");

	auto teamsNode = m_root.AddArray("teams");
	for (auto& pTeam : game.GetTeams())
	{
		VERIFY_MODEL_MSG("Team not chosen yet", pTeam->GetRace() != RaceType::None);

		std::string blueprints;
		switch (pTeam->GetRace())
		{
		case RaceType::Eridani:
		case RaceType::Planta:
		case RaceType::Orion:
			blueprints = ::EnumToString(pTeam->GetRace());
			break;
		default:
			blueprints = "normal";
		}

		auto teamNode = teamsNode.AppendElement();
		teamNode.SetAttribute("name", pTeam->GetPlayer().GetName());
		teamNode.SetAttribute("id", pTeam->GetPlayerID());
		teamNode.SetAttribute("blueprints", blueprints);
	}
}

UpdateTeam::UpdateTeam(const Team& team) : Update("team")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());
	m_root.SetAttribute("name", team.GetPlayer().GetName());
	m_root.SetAttribute("race", ::EnumToString(team.GetRace()));
	m_root.SetAttribute("colour", ::EnumToString(team.GetColour()));
}

UpdateInfluenceTrack::UpdateInfluenceTrack(const Team& team) : Update("influence_track")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());
	m_root.SetAttribute("discs", team.GetInfluenceTrack().GetDiscCount());
}

UpdateActionTrack::UpdateActionTrack(const Team& team) : Update("action_track")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());
	m_root.SetAttribute("discs", team.GetActionTrack().GetDiscCount());
}

UpdateStorageTrack::UpdateStorageTrack(const Team& team) : Update("storage_track")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());
	for (auto r : EnumRange<Resource>())
		m_root.SetAttribute(::EnumToString(r), team.GetStorage()[r]);
}

UpdateTechnologyTrack::UpdateTechnologyTrack(const Team& team) : Update("technology_track")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());
	auto classesNode = m_root.AddArray("classes");
	for (auto c : EnumRange<Technology::Class>())
	{
		auto eClass = classesNode.AppendElement();
		eClass.SetAttribute("name", ::EnumToString(c));
		auto techsNode = eClass.AddArray("techs");
		for (auto& t : team.GetTechTrack().GetClass(c))
			techsNode.AppendElement().SetAttribute("name", ::EnumToString(t));
	}
}

UpdatePopulationTrack::UpdatePopulationTrack(const Team& team) : Update("population_track")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());
	for (auto r : EnumRange<Resource>())
		m_root.SetAttribute(::EnumToString(r), team.GetPopulationTrack().GetPopulation()[r]);
}

UpdateColonyShips::UpdateColonyShips(const Team& team) : Update("colony_ships")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());
	m_root.SetAttribute("used", team.GetUnusedColonyShips());
	m_root.SetAttribute("total", team.GetColonyShips());
}

UpdateReputationTrack::UpdateReputationTrack(const Team& team, bool bSendValues) : Update("reputation_track")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());
	m_root.SetAttribute("tiles", team.GetReputationTrack().GetReputationTileCount());
	m_root.SetAttribute("slots", team.GetReputationTrack().GetSlotCount());
	m_root.SetAttribute("send_values", bSendValues);
}

UpdatePassed::UpdatePassed(const Team& team) : Update("passed")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());
	m_root.SetAttribute("has_passed", team.HasPassed());
}

UpdateBlueprints::UpdateBlueprints(const Team& team) : Update("blueprints")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());

	auto blueprintsNode = m_root.AddArray("blueprints");
	for (auto type : PlayerShipTypesRange())
	{
		auto partsNode = blueprintsNode.AppendArray();
		for (ShipPart part : team.GetBlueprint(type).GetOverlay().GetSlotRange())
			partsNode.Append(::EnumToString(part));
	}
}


UpdateMap::UpdateMap(const Game& game) : Update("map")
{
	const Map& map = game.GetMap();
	auto& hexes = map.GetHexes();

	auto hexesNode = m_root.AddArray("hexes");
	for (auto& i : hexes)
	{
		const MapPos& pos = i.first;
		const Hex& hex = *i.second.get();

		auto e = hexesNode.AppendElement();
		e.SetAttribute("x", pos.GetX());
		e.SetAttribute("y", pos.GetY());
		e.SetAttribute("id", hex.GetID());
		e.SetAttribute("rotation", hex.GetRotation());

		if (hex.IsOwned())
		{
			const Team& team = game.GetTeam(hex.GetColour());
			e.SetAttribute("colour", ::EnumToString(team.GetColour()));
		
			auto eSquares = e.AddArray("squares");
			for (auto& square : hex.GetSquares())
				if (square.IsOccupied())
				{
					auto eSquare = eSquares.AppendElement();
					eSquare.SetAttribute("x", square.GetX());
					eSquare.SetAttribute("y", square.GetY());
				}
		}

		auto eShips = e.AddArray("ships");
		for (auto& fleet : hex.GetFleets())
			for (auto& squadron : fleet.GetSquadrons())
			{
				auto eShip = eShips.AppendElement();
				eShip.SetAttribute("colour", ::EnumToString(fleet.GetColour()));
				eShip.SetAttribute("type", ::EnumToString(squadron.GetType()));
			}

		//DiscoveryType GetDiscoveryTile() const { return m_discovery; }
		//int GetVictoryPoints() const { return m_nVictory; }
	}
}

UpdateReviewUI::UpdateReviewUI(const ReviewGame& game) : Update("review_ui")
{
	m_root.SetAttribute("can_advance", game.CanAdvance());
	m_root.SetAttribute("can_retreat", game.CanRetreat());
	m_root.SetAttribute("next_record_id", game.GetNextRecordID());
}

UpdateTechnologies::UpdateTechnologies(const Game& game) : Update("technologies")
{
	auto techsNode = m_root.AddArray("techs");
	auto& techs = game.GetTechnologies();
	for (auto t : techs)
		if (t.second > 0)
		{
			auto e = techsNode.AppendElement();
			e.SetAttribute("type", ::EnumToString(t.first));
			e.SetAttribute("count", t.second);
			e.SetAttribute("max_cost", Technology::GetMaxCost(t.first));
			e.SetAttribute("min_cost", Technology::GetMinCost(t.first));
		}
}

UpdateRound::UpdateRound(const Game& game) : Update("round")
{
	m_root.SetAttribute("round", game.GetRound() + 1);
}

UpdateShowCombat::UpdateShowCombat(const Game& game, bool show) : Update("show_combat")
{
	m_root.SetAttribute("show", show);
}

UpdateCombat::UpdateCombat(const Game& game, const Battle& battle) : Update("combat")
{
	const Battle::Group& currentGroup = battle.GetCurrentGroup();

	Json::Element elems[] = { m_root.AddElement("defender"), m_root.AddElement("invader") };
	Json::Array groupsElems[] = { elems[0].AddArray("ship_groups"), elems[1].AddArray("ship_groups") };

	for (int invader = 0; invader < 2; ++invader)
		elems[invader].SetAttribute("colour", ::EnumToString(battle.GetColour(!!invader)));

	for (auto& group : battle.GetGroups())
	{
		auto groupElem = groupsElems[group.invader].AppendElement();
		groupElem.SetAttribute("type", ::EnumToString(group.shipType));
		groupElem.SetAttribute("max_lives", battle.GetBlueprint(game, group).GetLives());
		groupElem.SetAttribute("active", group.invader == currentGroup.invader && group.shipType == currentGroup.shipType);

		auto shipsElem = groupElem.AddArray("ships");
		for (auto& lives : group.lifeCounts)
			shipsElem.Append(lives);
	}
}

AddLog::AddLog(int id, const std::string& msg) : AddLog(Vec { Vec::value_type(id, msg) } )
{
}

AddLog::AddLog(const Vec& msgs) : Update("add_log")
{
	auto msgsNode = m_root.AddArray("items");
	for (auto& msg : msgs)
	{
		auto msgNode = msgsNode.AppendElement();
		msgNode.SetAttribute("id", msg.first);
		msgNode.SetAttribute("message", msg.second);
	}
}

RemoveLog::RemoveLog(int id) : Update("remove_log")
{
	m_root.SetAttribute("id", id);
}

//UpdateUndo::UpdateUndo(bool bEnable) : Update("undo")
//{
//	m_root.SetAttribute("enable", bEnable);
//}

ShowGameList::ShowGameList() :	Show("game_list_panel") {}
ShowChoose::ShowChoose() :		Show("choose_panel") {}
ShowLobby::ShowLobby() :		Show("lobby_panel") {}
ShowGame::ShowGame() :			Show("game_panel") {}

ChooseTeam::ChooseTeam(const Game& game, bool bActive) : Choose("team")
{
	m_root.SetAttribute("active", bActive);

	if (!bActive)
		return;

	auto teamsNode = m_root.AddArray("teams");
	for (auto c : EnumRange<Colour>())
		if (!game.FindTeam(c))
		{
			auto e = teamsNode.AppendElement();
			e.SetAttribute("name", ::EnumToString(RaceType::Human));
			e.SetAttribute("colour", ::EnumToString(c));
		}
	
	for (auto r : EnumRange<RaceType>())
		if (r != RaceType::Human)
		{
			Colour c = Race(r).GetColour();
			if (!game.FindTeam(c))
			{
				auto e = teamsNode.AppendElement();
				e.SetAttribute("name", ::EnumToString(r));
				e.SetAttribute("colour", ::EnumToString(c));
			}
		}
}

ChooseAction::ChooseAction(const LiveGame& game) : Choose("action") 
{
	const ActionPhase& phase = game.GetActionPhase();

	VERIFY_MODEL(!phase.GetCurrentCmd());

	bool bCanDoAction = phase.CanDoAction();
	bool bPassed = phase.GetCurrentTeam().HasPassed();

	m_root.SetAttribute("can_undo",			phase.CanRemoveCmd());
	m_root.SetAttribute("can_explore",		bCanDoAction && !bPassed);
	m_root.SetAttribute("can_influence",	bCanDoAction && !bPassed);
	m_root.SetAttribute("can_research",		bCanDoAction && !bPassed);
	m_root.SetAttribute("can_upgrade",		bCanDoAction);
	m_root.SetAttribute("can_build",		bCanDoAction);
	m_root.SetAttribute("can_move",			bCanDoAction);
	m_root.SetAttribute("can_colonise",		phase.GetCurrentTeam().GetUnusedColonyShips() > 0); 
	m_root.SetAttribute("can_diplomacy",	true);
	m_root.SetAttribute("can_trade",		true);
	m_root.SetAttribute("can_pass",			!phase.HasDoneAction() && !bPassed);
	m_root.SetAttribute("can_end_turn",		!bCanDoAction || bPassed);
}

ChooseFinished::ChooseFinished() : Choose("finished") 
{
}

ChooseExplorePos::ChooseExplorePos(const std::vector<MapPos>& positions, bool bCanSkip) : Choose("explore_pos") 
{
	m_root.SetAttribute("can_skip", bCanSkip);

	auto positionsNode = m_root.AddArray("positions");
	for (auto& pos : positions)
		AppendPointElement(pos.GetX(), pos.GetY(), positionsNode);
}

ChooseExploreHex::ChooseExploreHex(int x, int y, bool bCanTake, bool bCanUndo) : Choose("explore_hex") 
{
	m_root.SetAttribute("x", x);
	m_root.SetAttribute("y", y);
	m_root.SetAttribute("can_take", bCanTake);
	m_root.SetAttribute("can_undo", bCanUndo);

	m_hexes = m_root.AddArray("hexes");
}

void ChooseExploreHex::AddHexChoice(int idHex, const std::vector<int>& rotations, bool bCanInfluence)
{
	auto eHex = m_hexes.AppendElement();
	eHex.SetAttribute("id", idHex);
	eHex.SetAttribute("can_influence", bCanInfluence);
	
	auto rotationsNode = eHex.AddArray("rotations");
	for (int r : rotations)
		rotationsNode.Append(r);
}

ChooseDiscovery::ChooseDiscovery(bool bCanUndo) : Choose("discovery") 
{
	m_root.SetAttribute("can_undo", bCanUndo);
}

ChooseColonisePos::ChooseColonisePos(const std::vector<MapPos>& hexes) : Choose("colonise_pos") 
{
	auto positionsNode = m_root.AddArray("positions");
	for (auto& hex : hexes)
		AppendPointElement(hex.GetX(), hex.GetY(), positionsNode);
}

ChooseColoniseSquares::ChooseColoniseSquares(const MapPos& pos, const EnumArray<SquareType, int>& squares, const Population& pop, int nShips) : Choose("colonise_squares")
{
	m_root.SetAttribute("ships", nShips);
	m_root.SetAttribute("x", pos.GetX());
	m_root.SetAttribute("y", pos.GetY());

	auto eCounts = m_root.AddElement("square_counts");
	for (auto t : EnumRange<SquareType>())
		eCounts.SetAttribute(::EnumToString(t), squares[t]);

	auto eCubes = m_root.AddElement("max_cubes");
	for (auto r : EnumRange<Resource>())
		eCubes.SetAttribute(::EnumToString(r), pop[r]);
}

ChooseInfluencePos::ChooseInfluencePos(const std::vector<MapPos>& positions, bool bEnableTrack, const std::string& param) : Choose(param) 
{
	m_root.SetAttribute("can_select_track", bEnableTrack);

	auto positionsNode = m_root.AddArray("positions");
	for (auto& pos : positions)
		AppendPointElement(pos.GetX(), pos.GetY(), positionsNode);
}

ChooseInfluenceSrc::ChooseInfluenceSrc(const std::vector<MapPos>& positions, bool bEnableTrack, int flips) :
	ChooseInfluencePos(positions, bEnableTrack, "influence_src") 
{
	m_root.SetAttribute("max_flips", flips);
}

ChooseInfluenceDst::ChooseInfluenceDst(const std::vector<MapPos>& positions, bool bEnableTrack) : 
	ChooseInfluencePos(positions, bEnableTrack, "influence_dst") {}

ChooseResearch::ChooseResearch(const std::vector<std::pair<TechType, int>>& techs, bool bCanSkip) : Choose("research")
{
	m_root.SetAttribute("can_skip", bCanSkip);

	auto techsNode = m_root.AddArray("techs");
	for (auto& it : techs)
	{
		auto e = techsNode.AppendElement();
		e.SetAttribute("type", ::EnumToString(it.first));
		e.SetAttribute("cost", it.second);
	}
}

ChooseResearchArtifact::ChooseResearchArtifact(int nArtifacts) : Choose("research_artifact")
{
	m_root.SetAttribute("count", nArtifacts);
}

ChooseBuild::ChooseBuild(const std::set<ShipType>& ships, const std::map<MapPos, std::pair<bool, bool>>& hexes, bool bCanSkip) : Choose("build")
{
	m_root.SetAttribute("can_skip", bCanSkip);

	auto ship_elems = m_root.AddArray("ships");
	for (auto s : ships)
		ship_elems.Append(::EnumToString(s));

	auto hex_elems = m_root.AddArray("hexes");
	for (auto& it : hexes)
	{
		auto e = hex_elems.AppendElement();
		AddPointElement(it.first.GetX(), it.first.GetY(), "pos", e);
		e.SetAttribute("can_build_orbital", it.second.first);
		e.SetAttribute("can_build_monolith", it.second.second);
	}
}

ChooseDiplomacy::ChooseDiplomacy() : Choose("diplomacy")
{
}

ChooseMoveSrc::ChooseMoveSrc(std::map<MapPos, std::set<ShipType>>& srcs, bool bCanSkip) : Choose("move_src")
{
	m_root.SetAttribute("can_skip", bCanSkip);

	auto hexesNode = m_root.AddArray("hexes");
	for (auto& it : srcs)
	{
		auto e = hexesNode.AppendElement();
		AddPointElement(it.first.GetX(), it.first.GetY(), "pos", e);

		auto shipsNode = e.AddArray("ships");
		for (auto s : PlayerShipTypesRange())
		{
			auto j = it.second.find(s);
			if (j != it.second.end())
				shipsNode.Append(::EnumToString(s));
		}
	}
}

ChooseMoveDst::ChooseMoveDst(const std::set<MapPos>& dsts) : Choose("move_dst")
{
	auto hexesNode = m_root.AddArray("hexes");
	for (auto& h : dsts)
		AppendPointElement(h.GetX(), h.GetY(), hexesNode);
}

ChooseUpgrade::ChooseUpgrade(const Team& team) : Choose("upgrade")
{
	auto appendPart = [](Json::Array& array, ShipPart part)
	{
		auto partNode = array.AppendElement();
		partNode.SetAttribute("name", ::EnumToString(part));
		partNode.SetAttribute("power_source", ShipLayout::GetPowerSource(part));
		partNode.SetAttribute("power_drain", ShipLayout::GetPowerDrain(part));
		partNode.SetAttribute("is_drive", ShipLayout::IsDrive(part));
	};

	m_root.SetAttribute("max_upgrades", team.HasPassed() ? 1 : Race(team.GetRace()).GetUpgradeRate());

	auto partsNode = m_root.AddArray("parts");
	for (auto part : EnumRange<ShipPart>())
		if (team.CanUseShipPart(part))
			appendPart(partsNode, part);

	auto blueprintsNode = m_root.AddArray("blueprints");
	for (auto type : PlayerShipTypesRange())
	{
		auto& bp = team.GetBlueprint(type);

		auto blueprintNode = blueprintsNode.AppendElement();
		blueprintNode.SetAttribute("fixed_power", bp.GetFixedPower());

		auto baseLayoutNode = blueprintNode.AddArray("base_layout");
		for (ShipPart part : bp.GetBaseLayout().GetSlotRange())
			appendPart(baseLayoutNode, part);

		auto overlayNode = blueprintNode.AddArray("overlay");
		for (ShipPart part : bp.GetOverlay().GetSlotRange())
			appendPart(overlayNode, part);
	}
}

ChooseTrade::ChooseTrade(const Team& team) : Choose("trade")
{
	const int rate = Race(team.GetRace()).GetTradeRate();
	auto available = m_root.AddElement("available");
	for (auto r : EnumRange<Resource>())
	{
		int storage = team.GetStorage()[r];
		if (storage >= rate)
			available.SetAttribute(::EnumToString(r), storage);
	}
	m_root.SetAttribute("rate", rate);
}

ChooseUpkeep::ChooseUpkeep(const Team& team, bool canUndo) : Choose("upkeep")
{
	m_root.SetAttribute("can_undo", canUndo);
	m_root.SetAttribute("can_colonise", team.GetUnusedColonyShips() > 0);
	m_root.SetAttribute("can_trade", true);
	m_root.SetAttribute("can_bankrupt", false);
}

ChooseCombat::ChooseCombat(const LiveGame& game) : Choose("combat")
{
	const CombatPhase& phase = game.GetCombatPhase();

	const Battle& battle = game.GetBattle();

	m_root.SetAttribute("missiles", battle.IsMissilePhase());
	m_root.SetAttribute("can_fire", true); // TODO: Check guns.
	m_root.SetAttribute("can_retreat", false); // TODO: Check escape routes. 
}

// This is a choose command for /player/, but an update for everyone else. 
ChooseDice::ChooseDice(const LiveGame& game, const Dice& dice, int activePlayerId) : Choose("dice")
{
	m_root.SetAttribute("active_player_id", activePlayerId);

	auto diceElem = m_root.AddArray("dice");
	for (auto& pair : dice)
	{
		auto typeElem = diceElem.AppendElement();
		typeElem.SetAttribute("colour", ::EnumToString(pair.first));
		auto valuesElem = typeElem.AddArray("values");
		for (auto& val : pair.second)
			valuesElem.Append(val);
	}
}

} // namespace


