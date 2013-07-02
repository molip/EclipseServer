#pragma once

#include "Cmd.h"

#include <deque>
#include <vector>

namespace Serial { class SaveNode; class LoadNode; }

class CmdStack
{
public:
	CmdStack();
	
	void StartCmd(CmdPtr& pCmd);	// Start new chain and add cmd to it.
	void AddCmd(CmdPtr& pCmd);		// Add cmd to existing chain (null closes the chain).
	Cmd* RemoveCmd();				// Returns cmd to undo.
	Cmd* GetCurrentCmd();
	const Cmd* GetCurrentCmd() const;
	void Clear();
	
	bool CanRemoveCmd() const;
	bool HasAction() const;

	void AssertValid() const;

	bool Purge();

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	class Chain;
	typedef std::unique_ptr<Chain> ChainPtr;
	struct Node
	{
		Node() {}
		Node(CmdPtr& _pCmd) : pCmd(std::move(_pCmd)) {}

		void Save(Serial::SaveNode& node) const;
		void Load(const Serial::LoadNode& node);

		CmdPtr pCmd;
		std::vector<ChainPtr> subchains;
	};

	typedef std::unique_ptr<Node> NodePtr;
	class Chain : public std::vector<NodePtr>
	{
	friend class CmdStack;
	public:
		bool IsOpen() const;
		void AddCmd(CmdPtr& pCmd, bool bStart);
		Cmd* RemoveCmd(); // Returns cmd to undo.
		Cmd* GetCurrentCmd();
		const Cmd* GetCurrentCmd() const { return const_cast<Chain*>(this)->GetCurrentCmd(); }
	
		bool CanRemoveCmd() const; 
		bool HasAction() const;
	
		void AssertValid() const;

		bool Purge();

		void Save(Serial::SaveNode& node) const;
		void Load(const Serial::LoadNode& node);

	private:
		Chain* GetOpenChild();
		Chain* GetLastChild();
		Node& GetLastNode();
	};
	
	std::vector<ChainPtr> m_chains;
};
