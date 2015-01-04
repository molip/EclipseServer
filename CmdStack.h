#pragma once

#include "Cmd.h"

#include <deque>
#include <vector>

namespace Serial { class SaveNode; class LoadNode; }

class CmdStack
{
public:
	CmdStack();
	
	void StartCmd(CmdPtr pCmd);	// Start new chain and add cmd to it.
	void AddCmd(CmdPtr pCmd, CmdPtr queued = nullptr);		// Add cmd to existing chain (null closes the chain).
	Cmd* RemoveCmd();				// Returns cmd to undo.
	Cmd* GetCurrentCmd();
	const Cmd* GetCurrentCmd() const;
	void Clear();
	
	bool CanRemoveCmd() const;

	void AssertValid() const;

	void Save(Serial::SaveNode& node) const;
	void Load(const Serial::LoadNode& node);

private:
	bool Purge();

	class Chain;
	typedef std::unique_ptr<Chain> ChainPtr;
	struct Node
	{
		Node() : queued(false) {}
		Node(CmdPtr& _pCmd, bool _queued) : pCmd(std::move(_pCmd)), queued(_queued) {}

		void Save(Serial::SaveNode& node) const;
		void Load(const Serial::LoadNode& node);

		CmdPtr pCmd;
		std::vector<ChainPtr> subchains;
		bool queued;
	};

	typedef std::unique_ptr<Node> NodePtr;
	class Chain : public std::vector<NodePtr>
	{
	friend class CmdStack;
	public:
		bool IsOpen() const;
		void AddCmd(CmdPtr& pCmd, bool bStart, bool bQueue = false);
		Cmd* RemoveCmd(); // Returns cmd to undo.
		Cmd* GetCurrentCmd();
		const Cmd* GetCurrentCmd() const { return const_cast<Chain*>(this)->GetCurrentCmd(); }
	
		bool CanRemoveCmd() const; 
	
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
