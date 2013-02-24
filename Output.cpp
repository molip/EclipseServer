#include "Output.h"
#include "App.h"
#include "Model.h"

namespace Output
{

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

UpdateGameList::UpdateGameList(const Model& model) : Update("game_list")
{
	for (auto& g : model.GetGames())
	{
		auto pGameNode = AddElement("game", *m_pRoot);
		pGameNode->SetAttribute("name", g->GetName());
		pGameNode->SetAttribute("owner", g->GetOwner());
		pGameNode->SetAttribute("started", g->HasStarted());
	
		for (auto& i : g->GetPlayers())
			if (i != g->GetOwner())
			{
				auto pPlayerNode = AddElement("player", *pGameNode);
				pPlayerNode->SetAttribute("name", i);
			}
	}
}

UpdateLobby::UpdateLobby(const Game& game) : Update("lobby")
{
	m_pRoot->SetAttribute("owner", game.GetOwner());
	m_pRoot->SetAttribute("game", game.GetName());
	for (auto& i : game.GetPlayers())
		if (i != game.GetOwner())
		{
			auto pPlayerNode = AddElement("player", *m_pRoot);
			pPlayerNode->SetAttribute("name", i);
		}
}

UpdateGame::UpdateGame(const Game& game) : Update("game")
{
	m_pRoot->SetAttribute("owner", game.GetOwner());
	m_pRoot->SetAttribute("game", game.GetName());
	for (auto& i : game.GetPlayers())
		if (i != game.GetOwner())
		{
			auto pPlayerNode = AddElement("player", *m_pRoot);
			pPlayerNode->SetAttribute("name", i);
		}
}

ShowGameList::ShowGameList() : Show("game_list_panel") {}
ShowGame::ShowGame() : Show("game_panel") {}
ShowLobby::ShowLobby() : Show("lobby_panel") {}

} // namespace


