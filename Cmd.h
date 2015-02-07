#pragma once

#include "Dynamic.h"

#include <memory>

class Controller;
class Player;
class Team;
class LiveGame;
class CommitSession;

enum class Colour;

namespace Input { class CmdMessage; }

class Cmd;
typedef std::unique_ptr<Cmd> CmdPtr;

class Record;
typedef std::unique_ptr<Record> RecordPtr;

class Cmd : public Dynamic
{
public:
	struct ProcessResult
	{
		ProcessResult(Cmd* _next = nullptr, Cmd* _queue = nullptr) : next(_next), queue(_queue) {}
		ProcessResult(ProcessResult&& rhs) : next(std::move(rhs.next)), queue(std::move(rhs.queue)) {}
		ProcessResult(const ProcessResult& rhs) = delete;
		const ProcessResult& operator=(const ProcessResult& rhs) = delete;
		CmdPtr next, queue;
	};
	
	Cmd();
	Cmd(Colour colour);
	virtual ~Cmd() {}
	
	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const {}
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) = 0;
	
	virtual bool IsAutoProcess() const { return false; } 
	virtual bool IsAction() const { return false; } 
	virtual bool CostsInfluence() const { return IsAction(); } 
	virtual std::string GetActionName() const;
	virtual int GetRecordCount() const { return m_recordCount; }
	virtual bool CanUndo() const { return true; } 
	
	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	void PopRecord(CommitSession& session);
	Colour GetColour() const { return m_colour; }

	Team& GetTeam(LiveGame& game);
	const Team& GetTeam(const LiveGame& game) const;

protected:
	Player& GetPlayer(LiveGame& game);
	const Player& GetPlayer(const LiveGame& game) const;

	void DoRecord(RecordPtr pRec, CommitSession& session);

	Colour m_colour;
	int m_recordCount;
};
