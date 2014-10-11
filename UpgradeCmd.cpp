#include "stdafx.h"
#include "UpgradeCmd.h"
#include "Output.h"
#include "Input.h"
#include "Controller.h"
#include "LiveGame.h"
#include "Record.h"
#include "CommitSession.h"

class UpgradeRecord : public TeamRecord
{
public:
	UpgradeRecord() {}
	UpgradeRecord(Colour colour, const Input::SlotChanges& changes) : TeamRecord(colour)
	{
		for (auto c : changes)
			m_changes.push_back(Change(c));
	}

private:
	struct Change
	{
		Change() : ship(ShipType::None), slot(-1), newPart(ShipPart::Empty), oldPart(ShipPart::Empty) {}
		Change(const Input::SlotChange& rhs) : ship(rhs.ship), slot(rhs.slot), newPart(rhs.part), oldPart(ShipPart::Empty) {}

		void Save(Serial::SaveNode& node) const
		{
			node.SaveEnum("ship", ship);
			node.SaveType("slot", slot);
			node.SaveEnum("new_part", newPart);
			node.SaveEnum("old_part", oldPart);
		}

		void Load(const Serial::LoadNode& node)
		{
			node.LoadEnum("ship", ship);
			node.LoadType("slot", slot);
			node.LoadEnum("new_part", newPart);
			node.LoadEnum("old_part", oldPart);
		}

		ShipType ship;
		int slot;
		ShipPart newPart, oldPart;
	};


	virtual void Save(Serial::SaveNode& node) const override
	{
		__super::Save(node);
		node.SaveCntr("changes", m_changes, Serial::ClassSaver());
	}

	virtual void Load(const Serial::LoadNode& node) override
	{
		__super::Load(node);
		node.LoadCntr("changes", m_changes, Serial::ClassLoader());
	}

	virtual void Apply(bool bDo, Game& game, const Controller& controller) override
	{
		Team& team = game.GetTeam(m_colour);
		for (auto& c : m_changes)
		{
			Blueprint& bp = team.GetBlueprint(c.ship);
			if (bDo)
			{
				c.oldPart = bp.GetSlot(c.slot);
				bp.SetSlot(c.slot, c.newPart);
			}
			else
				bp.SetSlot(c.slot, c.oldPart);
		}
		controller.SendMessage(Output::UpdateBlueprints(team), game);
	}

	virtual std::string GetTeamMessage() const
	{
		return FormatString("upgraded");
	}
	std::vector<Change> m_changes;
};

REGISTER_DYNAMIC(UpgradeRecord)

//-----------------------------------------------------------------------------

UpgradeCmd::UpgradeCmd(Colour colour, const LiveGame& game) : Cmd(colour)
{
}

void UpgradeCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	controller.SendMessage(Output::ChooseUpgrade(GetTeam(game)), GetPlayer(game));
}

CmdPtr UpgradeCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdUpgrade>(msg);
	
	const Team& team = GetTeam(session.GetGame());

	const int allowedUpgrades = team.HasPassed() ? 1 : Race(team.GetRace()).GetUpgradeRate();
	VERIFY_INPUT_MSG("too many upgrades", (int)m.m_changes.size() <= allowedUpgrades);

	// Apply changes to temporary blueprints and validate them.
	std::vector<BlueprintPtr> blueprints;
	for (ShipType type : PlayerShipTypesRange())
		blueprints.push_back(BlueprintPtr(new Blueprint(team.GetBlueprint(type))));

	for (auto& c : m.m_changes)
		blueprints[(int)c.ship]->SetSlot(c.slot, c.part);

	for (auto& bp : blueprints)
		VERIFY_INPUT_MSG("invalid blueprint", bp->IsValid());

	UpgradeRecord* pRec = new UpgradeRecord(m_colour, m.m_changes);
	DoRecord(RecordPtr(pRec), session);

	return nullptr;
}

void UpgradeCmd::Save(Serial::SaveNode& node) const 
{
	__super::Save(node);
}

void UpgradeCmd::Load(const Serial::LoadNode& node) 
{
	__super::Load(node);
}

REGISTER_DYNAMIC(UpgradeCmd)
