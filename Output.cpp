#include "Output.h"
#include "App.h"
#include "Model.h"
#include "Team.h"
#include "Race.h"
#include "EnumRange.h"
#include "Player.h"
#include "Technology.h"
#include "EnumTraits.h"

namespace Output
{

std::string GetRaceName(RaceType race)
{
	switch (race)
	{
	case RaceType::Eridani:		return "eridani";
	case RaceType::Hydran:		return "hydran";
	case RaceType::Planta:		return "planta";
	case RaceType::Descendants:	return "descendants";
	case RaceType::Mechanema:	return "mechanema";
	case RaceType::Orion:		return "orion";
	case RaceType::Human:		return "human";
	};
	assert(false);
	return "";
}

std::string GetColourName(Colour colour)
{
	switch (colour)
	{
	case Colour::Black:		return "black";
	case Colour::Blue:		return "blue";
	case Colour::Green:		return "green";
	case Colour::Red:		return "red";
	case Colour::White:		return "white";
	case Colour::Yellow:	return "yellow";
	};
	assert(false);
	return "";
}

std::string GetResourceName(Resource type)
{
	switch (type)
	{
	case SquareType::Materials:	return "materials";
	case SquareType::Money:		return "money";
	case SquareType::Science:	return "science";
	};
	assert(false);
	return "";
}

std::string GetSquareTypeName(SquareType type)
{
	switch (type)
	{
	case SquareType::Materials:	return "materials";
	case SquareType::Money:		return "money";
	case SquareType::Science:	return "science";
	case SquareType::Any:		return "any";
	case SquareType::Orbital:	return "orbital";
	};
	assert(false);
	return "";
}

std::string GetTechClassName(Technology::Class type)
{
	switch (type)
	{
	case Technology::Class::Grid:		return "grid";
	case Technology::Class::Military:	return "military";
	case Technology::Class::Nano:		return "nano";
	};
	assert(false);
	return "";
}

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

Choose::Choose(const std::string& param, bool bActive) : Command("choose")
{
	m_root.SetAttribute("param", param);
	m_root.SetAttribute("active", bActive);
}

UpdateGameList::UpdateGameList(const Model& model) : Update("game_list")
{
	for (auto& g : model.GetGames())
	{
		auto gameNode = m_root.AddElement("game");
		gameNode.SetAttribute("name", g->GetName());
		gameNode.SetAttribute("id", g->GetID());
		gameNode.SetAttribute("owner", g->GetOwner().GetName());
		gameNode.SetAttribute("started", g->HasStarted());
	
		for (auto& i : g->GetTeams())
			if (i.first != &g->GetOwner())
			{
				auto playerNode = gameNode.AddElement("player");
				playerNode.SetAttribute("name", i.first->GetName());
			}
	}
}

UpdateLobby::UpdateLobby(const Game& game) : Update("lobby")
{
	m_root.SetAttribute("owner", game.GetOwner().GetName());
	m_root.SetAttribute("game", game.GetName());
	for (auto& i : game.GetTeams())
		if (i.first != &game.GetOwner())
		{
			auto pPlayerNode = m_root.AddElement("player");
			pPlayerNode.SetAttribute("name", i.first->GetName());
		}
}

UpdateLobbyControls::UpdateLobbyControls(bool bShow) : Update("lobby_controls")
{
	m_root.SetAttribute("show", bShow);
}

UpdateChoose::UpdateChoose(const Game& game) : Update("choose_team")
{
	m_root.SetAttribute("game", game.GetName());
	for (auto& i : game.GetTeamOrder())
	{
		auto pTeamNode = m_root.AddElement("team");
		pTeamNode.SetAttribute("name", i->GetName());
		if (game.HasTeamChosen(*i))
		{
			const Team& team = game.GetTeam(*i);
			pTeamNode.SetAttribute("race", GetRaceName(team.GetRace()));
			pTeamNode.SetAttribute("colour", GetColourName(team.GetColour()));
		}
	}
}

UpdateTeams::UpdateTeams(const Game& game) : Update("teams")
{
	AssertThrow("UpdateTeams: Game not started yet: " + game.GetName(), game.GetPhase() == Game::Phase::Main);

	m_root.SetAttribute("teams", game.GetName());
	for (Player* pPlayer : game.GetTeamOrder())
	{
		AssertThrow("UpdateTeams: Team not chosen yet: " + pPlayer->GetName(), game.HasTeamChosen(*pPlayer));

		auto pTeamNode = m_root.AddElement("team");
		pTeamNode.SetAttribute("name", pPlayer->GetName());
		pTeamNode.SetAttribute("id", pPlayer->GetID());
	}
}

UpdateTeam::UpdateTeam(const Team& team) : Update("team")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());
	m_root.SetAttribute("name", team.GetPlayer().GetName());
	m_root.SetAttribute("race", GetRaceName(team.GetRace()));
	m_root.SetAttribute("colour", GetColourName(team.GetColour()));
}

UpdateInfluenceTrack::UpdateInfluenceTrack(const Team& team) : Update("influence_track")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());
	m_root.SetAttribute("discs", team.GetInfluenceTrack().GetDiscCount());
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
			eTech.SetAttribute("name", EnumTraits<TechType>::ToString(t.GetType()));
		}
	}
}

UpdatePopulationTrack::UpdatePopulationTrack(const Team& team) : Update("population_track")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());
	for (auto r : EnumRange<Resource>())
		m_root.SetAttribute(EnumTraits<Resource>::ToString(r), team.GetPopulationTrack().GetPopulation()[r]);
}

UpdateReputationTrack::UpdateReputationTrack(const Team& team, bool bSendValues) : Update("reputation_track")
{
	m_root.SetAttribute("id", team.GetPlayer().GetID());
	m_root.SetAttribute("tiles", team.GetReputationTrack().GetReputationTileCount());
	m_root.SetAttribute("slots", team.GetReputationTrack().GetSlotCount());
	m_root.SetAttribute("send_values", bSendValues);
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

		if (const Team* pTeam = hex.GetOwner())
		{
			e.SetAttribute("colour", GetColourName(pTeam->GetColour()));
		
			auto eSquares = e.AddElement("squares");
			for (auto& square : hex.GetSquares())
				if (Team* pSquareOwner = square.GetOwner())
				{
					AssertThrow("UpdateMap::UpdateMap", pSquareOwner == pTeam);
					auto eSquare = eSquares.AddElement("square");
					eSquare.SetAttribute("x", square.GetX());
					eSquare.SetAttribute("y", square.GetY());
				}
		}


		//const std::vector<Square> GetSquares() const { return m_squares; }
		//std::vector<Ship> GetShips() const { return m_ships; }
		//DiscoveryType GetDiscoveryTile() const { return m_discovery; }
		//int GetVictoryPoints() const { return m_nVictory; }
		//bool HasArtifact() const { return m_bArtifact; }
	}
}

//UpdateUndo::UpdateUndo(bool bEnable) : Update("undo")
//{
//	m_root.SetAttribute("enable", bEnable);
//}

ShowGameList::ShowGameList() :	Show("game_list_panel") {}
ShowChoose::ShowChoose() :		Show("choose_panel") {}
ShowGame::ShowGame() :			Show("game_panel") {}
ShowLobby::ShowLobby() :		Show("lobby_panel") {}

ChooseTeam::ChooseTeam(const Game& game, bool bActive) : Choose("team", bActive)
{
	if (!bActive)
		return;

	for (auto c : EnumRange<Colour>())
		if (!game.GetTeamFromColour(c))
		{
			auto e = m_root.AddElement("race");
			e.SetAttribute("name", GetRaceName(RaceType::Human));
			e.SetAttribute("colour", GetColourName(c));
		}

	for (auto r : EnumRange<RaceType>())
		if (r != RaceType::Human)
		{
			Colour c = Race(r).GetColour();
			if (!game.GetTeamFromColour(c))
			{
				auto e = m_root.AddElement("race");
				e.SetAttribute("name", GetRaceName(r));
				e.SetAttribute("colour", GetColourName(c));
			}
		}
}

ChooseAction::ChooseAction(const Game& game) : Choose("action") 
{
	AssertThrow("ChooseAction::ChooseAction", !game.GetCurrentCmd());

	bool bCanDoAction = game.CanDoAction();

	m_root.SetAttribute("can_undo",		game.CanRemoveCmd());
	m_root.SetAttribute("can_explore",	bCanDoAction);
	m_root.SetAttribute("can_influence",	bCanDoAction);
	m_root.SetAttribute("can_research",	false);
	m_root.SetAttribute("can_upgrade",	false);
	m_root.SetAttribute("can_build",		false);
	m_root.SetAttribute("can_move",		false);
	m_root.SetAttribute("can_colonise",	game.GetCurrentTeam().GetUnusedColonyShips() > 0); 
	m_root.SetAttribute("can_diplomacy",	false);
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
	for (int i = 0; i < (int)SquareType::_Count; ++i)
	{
		auto eType = eCounts.AddElement("type");
		eType.SetAttribute("name", GetSquareTypeName(SquareType(i)));
		eType.SetAttribute("count", squares[i]);
	}
	auto eCubes = m_root.AddElement("max_cubes");
	for (auto r : EnumRange<Resource>())
	{
		auto eType = eCubes.AddElement("type");
		eType.SetAttribute("name", GetResourceName(r));
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

} // namespace


