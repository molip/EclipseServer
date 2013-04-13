#pragma once

#include <memory>

class Controller;
namespace Input { class CmdMessage; }

class Cmd
{
public:
	virtual ~Cmd() {}
	virtual void AcceptMessage(const Input::CmdMessage& msg) = 0;
	virtual void UpdateClient(const Controller& controller) const {}
	virtual bool IsFinished() const = 0;
	virtual bool CanUndo() { return false; }
	virtual bool Undo() { return false; } // Return true to delete cmd.

private:
};

typedef std::unique_ptr<Cmd> CmdPtr;