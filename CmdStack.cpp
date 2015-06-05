#include "stdafx.h"
#include "CmdStack.h"
#include "App.h"

bool CmdStack::Chain::IsOpen() const
{
	VERIFY_MODEL_MSG("empty chain", !empty());
	bool bOpen = back()->pCmd != nullptr;
	VERIFY_MODEL_MSG("chain closed but subchain open", bOpen || !const_cast<Chain*>(this)->GetOpenChild());
	return bOpen;
}

CmdStack::Node& CmdStack::Chain::GetLastNode()
{
	VERIFY_MODEL(!empty());
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
			VERIFY_MODEL_MSG("subchain open but chain closed", IsOpen());
			return pChild;
		}
	return nullptr;
}

void CmdStack::Chain::AddCmd(CmdPtr& pCmd, bool bStart, bool bQueue)
{
	Chain* pChain = this;
	if (empty()) // New root chain. 
	{
		VERIFY_MODEL_MSG("nothing to add to", bStart);
	}
	else
	{
		VERIFY_MODEL_MSG("chain not open", IsOpen());

		if (Chain* pChild = GetOpenChild())
		{
			pChild->AddCmd(pCmd, bStart, bQueue);
			return;
		}

		if (bStart) // Start a new subchain.
		{
			GetLastNode().subchains.push_back(ChainPtr(new Chain()));
			pChain = GetLastNode().subchains.back().get();
		}
	}
	pChain->push_back(NodePtr(new Node(pCmd, bQueue)));
}

Cmd* CmdStack::Chain::RemoveCmd() 
{
	Cmd* undo = nullptr;

	Chain* pChild = GetLastChild();
	if (pChild && !(back()->queued && pChild->size() == 1)) // Otherwise, just remove queued.
	{
		undo = pChild->RemoveCmd();
		if (pChild->empty()) // Delete empty child.
			back()->subchains.pop_back(); 
	}
	else
	{
		pop_back(); // Abort current cmd, or reopen chain if closed. 
		undo = empty() ? nullptr : back()->pCmd.get(); 
	}
	VERIFY(!undo || undo->CanUndo());
	return undo;
}

// Can we undo the command before the current one? 
bool CmdStack::Chain::CanRemoveCmd() const
{
	VERIFY_MODEL_MSG("empty chain", !empty());

	if (const Chain* pChild = const_cast<CmdStack::Chain*>(this)->GetLastChild())
		if (!(back()->queued && pChild->size() == 1)) // Otherwise, queued would be removed.
			return pChild->CanRemoveCmd();
	
	if (size() == 1)
		return back()->pCmd->CanUnstart();

	return (*(end() - 2))->pCmd->CanUndo();
}

void CmdStack::Chain::AssertValid() const
{
	VERIFY_MODEL(!empty());

	for (auto& node : *this)
	{
		bool bLast = &node == &back();
		if (node->pCmd)
		{
			for (auto& chain : node->subchains)
			{
				if (chain->IsOpen())
					VERIFY_MODEL(bLast && &chain == &node->subchains.back());
				chain->AssertValid();
			}
		}
		else
		{
			VERIFY_MODEL(bLast);
			VERIFY_MODEL(node->subchains.empty());
		}
	}
}

bool CmdStack::Chain::Purge() 
{
	for (auto n = rbegin(); n != rend(); ++n)
		if ((*n)->pCmd && !(*n)->pCmd->CanUndo())
		{
			erase(begin(), (n+1).base());
			return true;
		}
	return false;
}

void CmdStack::Chain::Save(Serial::SaveNode& node) const
{
	if (!empty())
		node.SaveCntr("nodes", *this, Serial::ClassPtrSaver());
}

void CmdStack::Chain::Load(const Serial::LoadNode& node)
{
	node.LoadCntr("nodes", *this, Serial::ClassPtrLoader());
}

//-----------------------------------------------------------------------------

CmdStack::CmdStack()
{
}

void CmdStack::StartCmd(CmdPtr pCmd)
{
	VERIFY_MODEL(!!pCmd);

	if (m_chains.empty() || !m_chains.back()->IsOpen())
		m_chains.push_back(ChainPtr(new Chain()));
	
	m_chains.back()->AddCmd(pCmd, true); 

	AssertValid();
	VERIFY_MODEL(!!GetCurrentCmd());
}

// queued: start a subchain pCmd, resume queued when subchain closed. 
void CmdStack::AddCmd(CmdPtr pCmd, CmdPtr queued)
{
	VERIFY_MODEL_MSG("no chains", !m_chains.empty());
	VERIFY_MODEL_MSG("chain not open", m_chains.back()->IsOpen());
	
	Purge();

	if (queued) 
	{
		// Add pCmd on subchain of queued.
		m_chains.back()->AddCmd(queued, false, true);
		m_chains.back()->AddCmd(pCmd, true);
	}
	else
		m_chains.back()->AddCmd(pCmd, false); 
	
	AssertValid();
	VERIFY_MODEL(!pCmd || GetCurrentCmd());
}

Cmd* CmdStack::RemoveCmd()
{
	VERIFY_MODEL(CanRemoveCmd());

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

bool CmdStack::Purge() 
{
	for (auto c = m_chains.rbegin(); c != m_chains.rend(); ++c)
		if ((*c)->Purge())
		{
			m_chains.erase(m_chains.begin(), (c+1).base());
			return true;
		}
	return false;
}

void CmdStack::Save(Serial::SaveNode& node) const
{
	if (!m_chains.empty())
		node.SaveCntr("chains", m_chains, Serial::ClassPtrSaver());
}

void CmdStack::Load(const Serial::LoadNode& node)
{
	node.LoadCntr("chains", m_chains, Serial::ClassPtrLoader());
}

//-----------------------------------------------------------------------------

void CmdStack::Node::Save(Serial::SaveNode& node) const
{
	node.SaveObject("cmd", pCmd);
	node.SaveType("queued", queued);
	if (!subchains.empty())
		node.SaveCntr("subchains", subchains, Serial::ClassPtrSaver());
}

void CmdStack::Node::Load(const Serial::LoadNode& node)
{
	node.LoadObject("cmd", pCmd);
	node.LoadType("queued", queued);
	node.LoadCntr("subchains", subchains, Serial::ClassPtrLoader());
}

