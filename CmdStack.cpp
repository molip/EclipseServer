#include "stdafx.h"
#include "CmdStack.h"
#include "App.h"

bool CmdStack::Chain::IsOpen() const
{
	AssertThrow("CmdStack::Chain::IsOpen: empty chain", !empty());
	bool bOpen = back()->pCmd != nullptr;
	AssertThrow("CmdStack::Chain::IsOpen: chain closed but subchain open", bOpen || !const_cast<Chain*>(this)->GetOpenChild());
	return bOpen;
}

CmdStack::Node& CmdStack::Chain::GetLastNode()
{
	AssertThrow("CmdStack::Chain::GetLastNode", !empty());
	return *back();
}

Cmd* CmdStack::Chain::GetCurrentCmd() 
{
	if (Chain* pChild = GetOpenChild())
		return pChild->GetCurrentCmd(); 

	return GetLastNode().pCmd.get();
}

CmdStack::Chain* CmdStack::Chain::GetLastChild() 
{
	Node& node = GetLastNode();
	if (!node.subchains.empty())
		return node.subchains.back().get();
	return nullptr;
}

CmdStack::Chain* CmdStack::Chain::GetOpenChild() 
{
	if (Chain* pChild = GetLastChild())
		if (pChild->IsOpen())
		{
			AssertThrow("CmdStack::Chain::GetOpenChild: subchain open but chain closed", IsOpen());
			return pChild;
		}
	return nullptr;
}

void CmdStack::Chain::AddCmd(CmdPtr& pCmd, bool bStart)
{
	Chain* pChain = this;
	if (empty()) // New root chain. 
	{
		AssertThrow("CmdStack::Chain::AddCmd: nothing to add to", bStart);
	}
	else
	{
		AssertThrow("CmdStack::Chain::AddCmd: chain not open", IsOpen());

		if (Chain* pChild = GetOpenChild())
		{
			pChild->AddCmd(pCmd, bStart); 
			return;
		}

		if (bStart) // Start a new subchain.
		{
			GetLastNode().subchains.push_back(ChainPtr(new Chain()));
			pChain = GetLastNode().subchains.back().get();
		}
	}
	pChain->push_back(NodePtr(new Node(pCmd)));
}

Cmd* CmdStack::Chain::RemoveCmd() 
{
	if (Chain* pChild = GetLastChild())
	{
		Cmd* pUndo = pChild->RemoveCmd();
		if (pChild->empty()) // Delete empty child.
			back()->subchains.pop_back();
		return pUndo;
	}
	pop_back(); // Abort current cmd, or reopen chain if closed. 
	return empty() ? nullptr : back()->pCmd.get(); // Don't undo if empty.
}

bool CmdStack::Chain::CanRemoveCmd() const
{
	if (const Chain* pChild = const_cast<CmdStack::Chain*>(this)->GetLastChild())
		return pChild->CanRemoveCmd();
	
	AssertThrow("CmdStack::Chain::CanRemoveCmd: empty chain", !empty());

	return size() == 1 || (*(end() - 2))->pCmd->CanUndo();
}

bool CmdStack::Chain::HasAction() const
{
	AssertThrow("CmdStack::Chain::HasAction: empty chain", !empty());
	return front()->pCmd->IsAction();
}

void CmdStack::Chain::AssertValid() const
{
	std::string s = "CmdStack::Chain::AssertValid";
	AssertThrow(s, !empty());

	for (auto& node : *this)
	{
		bool bLast = &node == &back();
		if (node->pCmd)
		{
			for (auto& chain : node->subchains)
			{
				if (chain->IsOpen())
					AssertThrow(s, bLast && &chain == &node->subchains.back());
				chain->AssertValid();
			}
		}
		else
		{
			AssertThrow(s, bLast);
			AssertThrow(s, node->subchains.empty());
		}
	}
}

//-----------------------------------------------------------------------------

CmdStack::CmdStack()
{
}

void CmdStack::StartCmd(CmdPtr& pCmd)
{
	AssertThrow("CmdStack::StartCmd", !!pCmd);

	if (m_chains.empty() || !m_chains.back()->IsOpen())
		m_chains.push_back(ChainPtr(new Chain()));
	
	m_chains.back()->AddCmd(pCmd, true); 

	AssertValid();
	AssertThrow("CmdStack::StartCmd", !!GetCurrentCmd());
}

void CmdStack::AddCmd(CmdPtr& pCmd)
{
	AssertThrow("CmdStack::AddCmd: no chains", !m_chains.empty());
	AssertThrow("CmdStack::AddCmd: chain not open", m_chains.back()->IsOpen());
	
	m_chains.back()->AddCmd(pCmd, false); 
	
	AssertValid();
	AssertThrow("CmdStack::AddCmd", !pCmd || GetCurrentCmd());
}

Cmd* CmdStack::RemoveCmd()
{
	AssertThrow("CmdStack::RemoveCmd", CanRemoveCmd());

	Cmd* pCmd = m_chains.back()->RemoveCmd();
	if (m_chains.back()->empty())
		m_chains.pop_back();

	AssertValid();
	return pCmd;
}

Cmd* CmdStack::GetCurrentCmd() 
{
	return m_chains.empty() ? nullptr : m_chains.back()->GetCurrentCmd();
}

const Cmd* CmdStack::GetCurrentCmd() const
{
	return m_chains.empty() ? nullptr : m_chains.back()->GetCurrentCmd();
}

bool CmdStack::HasAction() const
{
	for (auto& chain : m_chains)
		if (chain->HasAction())
			return true;
	return false;
}

bool CmdStack::CanRemoveCmd() const
{
	return !m_chains.empty() && m_chains.back()->CanRemoveCmd();
}

void CmdStack::Clear()
{
	m_chains.clear();
}

void CmdStack::AssertValid() const
{
	for (auto& chain : m_chains)
		chain->AssertValid();
}