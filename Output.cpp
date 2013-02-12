#include "Output.h"
#include "App.h"
#include "Model.h"

namespace Output
{

const std::string& Message::GetXML() const
{
	assert(!m_pPrinter);
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
	
		for (auto& i : g->GetPlayers())
		{
			auto pPlayerNode = AddElement("player", *pGameNode);
			pPlayerNode->SetAttribute("name", i.first);
		}
	}
}

ShowGameList::ShowGameList() : Show("game_list_panel") {}

} // namespace


