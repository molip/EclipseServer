#include "Output.h"
#include "App.h"
#include "Model.h"
#include "Team.h"
#include "Race.h"
#include "EnumRange.h"

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

const std::string& Message::GetXML() const
{
	ASSERT(!m_pPrinter);
	//Create();
	m_pPrinter.reset(new TiXmlPrinter);
	m_doc.Accept(m_pPrinter.get());
	return m_pPrinter->Str();
}

TiXmlElement* Message::AddElement(const std::string& name, TiXmlNode& parent)
{
	TiXmlElement* pElem = new TiXmlElement(name);
	parent.LinkEndChild(pElem);
	return pElem;
}

//--------------------------------------------

Command::Command(const std::string& cmd) 
{
	m_pRoot = AddElement("command");
	m_pRoot->SetAttribute("type", cmd);
}

Show::Show(const std::string& panel) : Command("show")
{
	m_pRoot->SetAttribute("panel", panel);
}

Update::Update(const std::string& param) : Command("update")
{
	m_pRoot->SetAttribute("param", param);
}

Action::Action(const std::string& param, bool bActive) : Command("action")
{
	m_pRoot->SetAttribute("param", param);
	m_pRoot->SetAttribute("active", bActive);
}

UpdateGameList::UpdateGameList(const Model& model) : Update("game_list")
{
	for (auto& g : model.GetGames())
	{
		auto pGameNode = AddElement("game", *m_pRoot);
		pGameNode->SetAttribute("name", g->GetName());
		pGameNode->SetAttribute("owner", g->GetOwner());
		pGameNode->SetAttribute("started", g->HasStarted());
	
		for (auto& i : g->GetTeams())
			if (i.first != g->GetOwner())
			{
				auto pPlayerNode = AddElement("player", *pGameNode);
				pPlayerNode->SetAttribute("name", i.first);
			}
	}
}

UpdateLobby::UpdateLobby(const Game& game) : Update("lobby")
{
	m_pRoot->SetAttribute("owner", game.GetOwner());
	m_pRoot->SetAttribute("game", game.GetName());
	for (auto& i : game.GetTeams())
		if (i.first != game.GetOwner())
		{
			auto pPlayerNode = AddElement("player", *m_pRoot);
			pPlayerNode->SetAttribute("name", i.first);
		}
}

UpdateLobbyControls::UpdateLobbyControls(bool bShow) : Update("lobby_controls")
{
	m_pRoot->SetAttribute("show", bShow);
}

UpdateChoose::UpdateChoose(const Game& game) : Update("choose_team")
{
	m_pRoot->SetAttribute("game", game.GetName());
	for (auto& i : game.GetTeamOrder())
	{
		auto pTeamNode = AddElement("team", *m_pRoot);
		pTeamNode->SetAttribute("name", i);
		if (game.HasTeamChosen(i))
		{
			const Team& team = game.GetTeam(i);
			pTeamNode->SetAttribute("race", GetRaceName(team.GetRace()));
			pTeamNode->SetAttribute("colour", GetColourName(team.GetColour()));
		}
	}
}

UpdateTeams::UpdateTeams(const Game& game) : Update("teams")
{
	AssertThrow("UpdateTeams: Game not started yet: " + game.GetName(), game.GetPhase() == Game::Phase::Main);

	m_pRoot->SetAttribute("teams", game.GetName());
	for (auto& name : game.GetTeamOrder())
	{
		AssertThrow("UpdateTeams: Team not chosen yet: " + name, game.HasTeamChosen(name));

		auto pTeamNode = AddElement("team", *m_pRoot);
		pTeamNode->SetAttribute("name", name);
	}
}

UpdateTeam::UpdateTeam(const Team& team) : Update("team")
{
	m_pRoot->SetAttribute("name", team.GetPlayer());
	m_pRoot->SetAttribute("race", GetRaceName(team.GetRace()));
	m_pRoot->SetAttribute("colour", GetColourName(team.GetColour()));
}

UpdateMap::UpdateMap(const Game& game) : Update("map")
{
	const Map& map = game.GetMap();
	auto& hexes = map.GetHexes();

	for (auto& i : hexes)
	{
		const MapPos& pos = i.first;
		const Hex& hex = *i.second.get();

		auto pNode = AddElement("hex", *m_pRoot);
		pNode->SetAttribute("x", pos.GetX());
		pNode->SetAttribute("y", pos.GetY());
		pNode->SetAttribute("id", hex.GetID());
		pNode->SetAttribute("rotation", hex.GetRotation());

		if (const Team* pTeam = hex.GetOwner())
			pNode->SetAttribute("colour", GetColourName(pTeam->GetColour()));


		//const std::vector<Square> GetSquares() const { return m_squares; }
		//std::vector<Ship> GetShips() const { return m_ships; }
		//DiscoveryType GetDiscoveryTile() const { return m_discovery; }
		//EdgeSet GetWormholes() const { return m_wormholes; }
		//int GetVictoryPoints() const { return m_nVictory; }
		//bool HasArtifact() const { return m_bArtifact; }
	}
}

ShowGameList::ShowGameList() :	Show("game_list_panel") {}
ShowChoose::ShowChoose() :		Show("choose_panel") {}
ShowGame::ShowGame() :			Show("game_panel") {}
ShowLobby::ShowLobby() :		Show("lobby_panel") {}

ActionChoose::ActionChoose(const Game& game, bool bActive) : Action("choose_team", bActive)
{
	if (!bActive)
		return;

	for (auto c : EnumRange<Colour>())
		if (!game.GetTeamFromColour(c))
		{
			auto e = AddElement("race", *m_pRoot);
			e->SetAttribute("name", GetRaceName(RaceType::Human));
			e->SetAttribute("colour", GetColourName(c));
		}

	for (auto r : EnumRange<RaceType>())
		if (r != RaceType::Human)
		{
			Colour c = Race(r).GetColour();
			if (!game.GetTeamFromColour(c))
			{
				auto e = AddElement("race", *m_pRoot);
				e->SetAttribute("name", GetRaceName(r));
				e->SetAttribute("colour", GetColourName(c));
			}
		}
}

} // namespace


