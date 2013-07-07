#pragma once

#include "Cmd.h"

#include <vector>

enum class TechType;

class ResearchCmd : public Cmd
{
public:
	ResearchCmd() {}
	ResearchCmd(Colour colour, LiveGame& game);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual CmdPtr Process(const Input::CmdMessage& msg, const Controller& controller, LiveGame& game) override;
	virtual void Undo(const Controller& controller, LiveGame& game) override;
	virtual bool IsAction() const { return true; } 
	virtual bool HasRecord() const { return true; } 

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	mutable std::vector<std::pair<TechType, int>> m_techs;
};

