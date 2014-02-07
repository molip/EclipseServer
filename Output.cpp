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

namespace Output
{

std::string Message::GetXML() const
{
	return m_doc.SaveToString();
}

//--------------------------------------------

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
	for (auto& g : Games::GetLiveGames())
	{
		auto gameNode = m_root.AddElement("game");
		gameNode.SetAttribute("name", g->GetName());
		gameNode.SetAttribute("id", g->GetID());
		gameNode.SetAttribute("owner", g->GetOwner().GetName());
		gameNode.SetAttribute("started", g->HasStarted());
	
		for (auto& i : g->GetTeams())
			if (i->GetPlayerID() != g->GetOwner().GetID())
			{
				auto playerNode = gameNode.AddElement("player");
				playerNode.SetAttribute("name", i->GetPlayer().GetName());
			}
	}
}

UpdateLobby::UpdateLobby(const Game& game) : Update("lobby")
{
	m_root.SetAttribute("owner", game.GetOwner().GetName());
	m_root.SetAttribute("game", game.GetName());
	for (auto& t : game.GetTeams())
		if (t->GetPlayerID() != game.GetOwner().GetID())
		{
			auto pPlayerNode = m_root.AddElement("player");
			pPlayerNode.SetAttribute("name", t->GetPlayer().GetName());
		}
}

UpdateLobbyControls::UpdateLobbyControls(bool bShow) : Update("lobby_controls")
{
	m_root.SetAttribute("show", bShow);
}

UpdateChoose::UpdateChoose(const LiveGame& game) : Update("choose_team")
{
	m_root.SetAttribute("game", game.GetName());
	for (auto& t : game.GetTeams())
	{
		auto pTeamNode = m_root.AddElement("team");
		pTeamNode.SetAttribute("name", t->GetPlayer().GetName());
		if (t->GetRace() != RaceType::None)
		{
			pTeamNode.SetAttribute("race", EnumTraits<RaceType>::ToString(t->GetRace()));
			pTeamNode.SetAttribute("colour", EnumTraits<Colour>::ToString(t->GetColour()));
		}
	}
}

UpdateTeams::UpdateTeams(const Game& game) : Update("teams")
{
	AssertThrow("UpdateTeams: Game not started yet: " + game.GetName(), game.HasStarted());

	m_root.SetAttribute("game_type", game.IsLive() ? "live" : "review");

	//m_root.SetAttribute("teams", game.GetName());
	for (auto& pTeam : game.GetTeams())
	{
		AssertThrow("UpdateTeams: Team not chosen yet: " + pTeam->GetPlayer().GetName(), pTeam->GetRace() != RaceType::None);

		auto pTeamNode = m_root.AddElement("team");
		pTeamNode.SetAttribute("name", pTeam->GetPlayer().GetName());
		pTeamNode.SetAttribute("id", pTeam->GetPlayerID());
	}
}

UpdateTeam::UpdateTeam(const Team& team) : Update("team")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());
	m_root.SetAttribute("name", team.GetPlayer().GetName());
	m_root.SetAttribute("race", EnumTraits<RaceType>::ToString(team.GetRace()));
	m_root.SetAttribute("colour", EnumTraits<Colour>::ToString(team.GetColour()));
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
		m_root.SetAttribute(EnumTraits<Resource>::ToString(r), team.GetStorage()[r]);
}

UpdateTechnologyTrack::UpdateTechnologyTrack(const Team& team) : Update("technology_track")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());
	for (auto c : EnumRange<Technology::Class>())
	{
		auto eClass = m_root.AddElement("class");
		eClass.SetAttribute("name", EnumTraits<Technology::Class>::ToString(c));
		for (auto& t : team.GetTechTrack().GetClass(c))
		{
			auto eTech = eClass.AddElement("tech");
			eTech.SetAttribute("name", EnumTraits<TechType>::ToString(t));
		}
	}
}

UpdatePopulationTrack::UpdatePopulationTrack(const Team& team) : Update("population_track")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());
	for (auto r : EnumRange<Resource>())
		m_root.SetAttribute(EnumTraits<Resource>::ToString(r), team.GetPopulationTrack().GetPopulation()[r]);
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

UpdateMap::UpdateMap(const Game& game) : Update("map")
{
	const Map& map = game.GetMap();
	auto& hexes = map.GetHexes();

	for (auto& i : hexes)
	{
		const MapPos& pos = i.first;
		const Hex& hex = *i.second.get();

		auto e = m_root.AddElement("hex");
		e.SetAttribute("x", pos.GetX());
		e.SetAttribute("y", pos.GetY());
		e.SetAttribute("id", hex.GetID());
		e.SetAttribute("rotation", hex.GetRotation());

		if (hex.IsOwned())
		{
			const Team& team = game.GetTeam(hex.GetColour());
			e.SetAttribute("colour", EnumTraits<Colour>::ToString(team.GetColour()));
		
			auto eSquares = e.AddElement("squares");
			for (auto& square : hex.GetSquares())
				if (square.IsOccupied())
				{
					auto eSquare = eSquares.AddElement("square");
					eSquare.SetAttribute("x", square.GetX());
					eSquare.SetAttribute("y", square.GetY());
				}
		}

		auto eShips = e.AddElement("ships");
		for (auto& ship : hex.GetShips())
		{
			auto eShip = eShips.AddElement("ship");
			eShip.SetAttribute("colour", EnumTraits<Colour>::ToString(ship.GetColour()));
			eShip.SetAttribute("type", EnumTraits<ShipType>::ToString(ship.GetType()));
		}

		//DiscoveryType GetDiscoveryTile() const { return m_discovery; }
		//int GetVictoryPoints() const { return m_nVictory; }
	}
}

UpdateReviewUI::UpdateReviewUI(const ReviewGame& game) : Update("review_ui")
{
	m_root.SetAttribute("can_advance", game.CanAdvance());
	m_root.SetAttribute("can_retreat", game.CanRetreat());
}

UpdateTechnologies::UpdateTechnologies(const Game& game) : Update("technologies")
{
	auto& techs = game.GetTechnologies();
	for (auto t : techs)
		if (t.second > 0)
		{
			auto e = m_root.AddElement("tech");
			e.SetAttribute("type", EnumTraits<TechType>::ToString(t.first));
			e.SetAttribute("count", t.second);
			e.SetAttribute("max_cost", Technology::GetMaxCost(t.first));
			e.SetAttribute("min_cost", Technology::GetMinCost(t.first));
		}
}

UpdateRound::UpdateRound(const Game& game) : Update("round")
{
	m_root.SetAttribute("round", game.GetRound() + 1);
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

	for (auto c : EnumRange<Colour>())
		if (!game.FindTeam(c))
		{
			auto e = m_root.AddElement("race");
			e.SetAttribute("name", EnumTraits<RaceType>::ToString(RaceType::Human));
			e.SetAttribute("colour", EnumTraits<Colour>::ToString(c));
		}

	for (auto r : EnumRange<RaceType>())
		if (r != RaceType::Human)
		{
			Colour c = Race(r).GetColour();
			if (!game.FindTeam(c))
			{
				auto e = m_root.AddElement("race");
				e.SetAttribute("name", EnumTraits<RaceType>::ToString(r));
				e.SetAttribute("colour", EnumTraits<Colour>::ToString(c));
			}
		}
}

ChooseAction::ChooseAction(const LiveGame& game) : Choose("action") 
{
	const ActionPhase& phase = game.GetActionPhase();

	AssertThrow("ChooseAction::ChooseAction", !phase.GetCurrentCmd());

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
	m_root.SetAttribute("can_pass",			bCanDoAction && !bPassed);
	m_root.SetAttribute("can_end_turn",		!bCanDoAction || bPassed);
}

ChooseFinished::ChooseFinished() : Choose("finished") 
{
}

ChooseExplorePos::ChooseExplorePos(const std::vector<MapPos>& positions, bool bCanSkip) : Choose("explore_pos") 
{
	m_root.SetAttribute("can_skip", bCanSkip);

	for (auto& pos : positions)
	{
		auto e = m_root.AddElement("pos");
		e.SetAttribute("x", pos.GetX());
		e.SetAttribute("y", pos.GetY());
	}
}

ChooseExploreHex::ChooseExploreHex(int x, int y, bool bCanTake, bool bCanUndo) : Choose("explore_hex") 
{
	m_root.SetAttribute("x", x);
	m_root.SetAttribute("y", y);
	m_root.SetAttribute("can_take", bCanTake);
	m_root.SetAttribute("can_undo", bCanUndo);
}

void ChooseExploreHex::AddHexChoice(int idHex, const std::vector<int>& rotations, bool bCanInfluence)
{
	auto eHex = m_root.AddElement("hex");
	eHex.SetAttribute("id", idHex);
	eHex.SetAttribute("can_influence", bCanInfluence);
	
	for (int r : rotations)
	{
		auto eRot = eHex.AddElement("rotation");
		eRot.SetAttribute("steps", r);
	}
}

ChooseDiscovery::ChooseDiscovery(bool bCanUndo) : Choose("discovery") 
{
	m_root.SetAttribute("can_undo", bCanUndo);
}

ChooseColonisePos::ChooseColonisePos(const std::vector<MapPos>& hexes) : Choose("colonise_pos") 
{
	for (auto& hex : hexes)
	{
		auto e = m_root.AddElement("pos");
		e.SetAttribute("x", hex.GetX());
		e.SetAttribute("y", hex.GetY());
	}
}

ChooseColoniseSquares::ChooseColoniseSquares(const int squares[SquareType::_Count], const Population& pop, int nShips) : Choose("colonise_squares") 
{
	m_root.SetAttribute("ships", nShips);

	auto eCounts = m_root.AddElement("square_counts");
	for (auto t : EnumRange<SquareType>())
	{
		auto eType = eCounts.AddElement("type");
		eType.SetAttribute("name", EnumTraits<SquareType>::ToString(t));
		eType.SetAttribute("count", squares[(int)t]);
	}
	auto eCubes = m_root.AddElement("max_cubes");
	for (auto r : EnumRange<Resource>())
	{
		auto eType = eCubes.AddElement("type");
		eType.SetAttribute("name", EnumTraits<Resource>::ToString(r));
		eType.SetAttribute("count", pop[r]);
	}
}

ChooseInfluencePos::ChooseInfluencePos(const std::vector<MapPos>& positions, bool bEnableTrack, const std::string& param) : Choose(param) 
{
	m_root.SetAttribute("can_select_track", bEnableTrack);

	for (auto& pos : positions)
	{
		auto e = m_root.AddElement("pos");
		e.SetAttribute("x", pos.GetX());
		e.SetAttribute("y", pos.GetY());
	}
}

ChooseInfluenceSrc::ChooseInfluenceSrc(const std::vector<MapPos>& positions, bool bEnableTrack) : 
	ChooseInfluencePos(positions, bEnableTrack, "influence_src") 
{
}

ChooseInfluenceDst::ChooseInfluenceDst(const std::vector<MapPos>& positions, bool bEnableTrack) : 
	ChooseInfluencePos(positions, bEnableTrack, "influence_dst") {}

ChooseResearch::ChooseResearch(const std::vector<std::pair<TechType, int>>& techs, bool bCanSkip) : Choose("research")
{
	m_root.SetAttribute("can_skip", bCanSkip);

	for (auto& it : techs)
	{
		auto e = m_root.AddElement("tech");
		e.SetAttribute("type", EnumTraits<TechType>::ToString(it.first));
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

	auto ship_elems = m_root.AddElement("ships");
	for (auto s : ships)
		ship_elems.AddElement("ship").SetAttribute("type", EnumTraits<ShipType>::ToString(s));

	auto hex_elems = m_root.AddElement("hexes");
	for (auto& it : hexes)
	{
		auto e = hex_elems.AddElement("hex");
		e.SetAttribute("x", it.first.GetX());
		e.SetAttribute("y", it.first.GetY());
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

	for (auto& it : srcs)
	{
		auto e = m_root.AddElement("hex");
		e.SetAttribute("x", it.first.GetX());
		e.SetAttribute("y", it.first.GetY());
		
		for (auto s : EnumRange<ShipType>())
		{
			auto j = it.second.find(s);
			if (j != it.second.end())
				e.AddElement("ship").SetAttribute("type", EnumTraits<ShipType>::ToString(s));
		}
	}
}

ChooseMoveDst::ChooseMoveDst(const std::set<MapPos>& dsts) : Choose("move_dst")
{
	for (auto& h : dsts)
	{
		auto e = m_root.AddElement("hex");
		e.SetAttribute("x", h.GetX());
		e.SetAttribute("y", h.GetY());
	}
}

ChooseUpgrade::ChooseUpgrade() : Choose("upgrade")
{
}

ChooseTrade::ChooseTrade() : Choose("trade")
{
}

ChooseUpkeep::ChooseUpkeep(const Team& team, bool canUndo) : Choose("upkeep")
{
	m_root.SetAttribute("can_undo", canUndo);
	m_root.SetAttribute("can_colonise", team.GetUnusedColonyShips() > 0);
	m_root.SetAttribute("can_trade", true);
	m_root.SetAttribute("can_bankrupt", false);
}

} // namespace


