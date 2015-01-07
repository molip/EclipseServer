#pragma once

#include "Cmd.h"
#include <vector>

enum class ShipPart;

class UpgradeCmd : public Cmd
{
public:
	UpgradeCmd() {}
	UpgradeCmd(Colour colour, const LiveGame& game);

	virtual void UpdateClient(const Controller& controller, const LiveGame& game) const override;
	virtual ProcessResult Process(const Input::CmdMessage& msg, CommitSession& session) override;
	virtual bool IsAction() const { return true; }
	virtual std::string GetActionName() const override { return "Upgrade"; }

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	virtual std::vector<ShipPart> GetParts(const Team& team) const;
	virtual int GetAllowedUpgradeCount(const Team& team) const;
	virtual bool CanRemoveParts() const { return true; }
};

class UpgradeDiscoveryCmd : public UpgradeCmd
{
public:
	UpgradeDiscoveryCmd() {}
	UpgradeDiscoveryCmd(Colour colour, const LiveGame& game, ShipPart part);

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

private:
	virtual std::vector<ShipPart> GetParts(const Team& team) const override;
	virtual int GetAllowedUpgradeCount(const Team& team) const override { return 1; }
	virtual bool CanRemoveParts() const override { return false; }

	ShipPart m_part;
};