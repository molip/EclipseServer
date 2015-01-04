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
		ProcessResult(Cmd* _next = nullptr) : next(_next) {}
		ProcessResult(ProcessResult&& rhs) : next(std::move(rhs.next)) {}
		ProcessResult(const ProcessResult& rhs) = delete;
		const ProcessResult& operator=(const ProcessResult& rhs) = delete;
		CmdPtr next;
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
	virtual bool HasRecord() const { return m_bHasRecord; }
	virtual bool CanUndo() const { return true; } 
	
	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	void PopRecord(CommitSession& session);
	Colour GetColour() const { return m_colour; }

protected:
	Team& GetTeam(LiveGame& game);
	const Team& GetTeam(const LiveGame& game) const;

	Player& GetPlayer(LiveGame& game);
	const Player& GetPlayer(const LiveGame& game) const;

	void DoRecord(RecordPtr pRec, CommitSession& session);

	Colour m_colour;
	bool m_bHasRecord;
};
