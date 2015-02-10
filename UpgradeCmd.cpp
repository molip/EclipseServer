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

	virtual void Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState) override
	{
		for (auto& c : m_changes)
		{
			Blueprint& bp = teamState.GetBlueprint(c.ship);
			bool ancient = ShipLayout::IsAncientShipPart(c.newPart);
			if (bDo)
			{
				c.oldPart = bp.GetSlot(c.slot);
				bp.SetSlot(c.slot, c.newPart);
				if (ancient)
					VERIFY(teamState.m_discoveredShipParts.erase(c.newPart) == 1);
			}
			else
			{
				bp.SetSlot(c.slot, c.oldPart);
				if (ancient)
					VERIFY(teamState.m_discoveredShipParts.insert(c.newPart).second);
			}
		}
	}

	virtual void Update(const Game& game, const Team& team, const RecordContext& context) const override
	{
		context.SendMessage(Output::UpdateBlueprints(team));
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

std::vector<ShipPart> UpgradeCmd::GetParts(const Team& team) const
{
	std::vector<ShipPart> parts;
	for (auto part : EnumRange<ShipPart>())
		if (team.CanUseShipPart(part))
			parts.push_back(part);
	return parts;
}

int UpgradeCmd::GetAllowedUpgradeCount(const Team& team) const
{
	return team.HasPassed() ? 1 : Race(team.GetRace()).GetUpgradeRate();
}

void UpgradeCmd::UpdateClient(const Controller& controller, const LiveGame& game) const
{
	const Team& team = GetTeam(game);
	controller.SendMessage(Output::ChooseUpgrade(team, GetParts(team), GetAllowedUpgradeCount(team), CanRemoveParts()), GetPlayer(game));
}

Cmd::ProcessResult UpgradeCmd::Process(const Input::CmdMessage& msg, CommitSession& session)
{
	auto& m = VerifyCastInput<const Input::CmdUpgrade>(msg);
	
	const Team& team = GetTeam(session.GetGame());

	VERIFY_INPUT_MSG("too many upgrades", (int)m.m_changes.size() <= GetAllowedUpgradeCount(team));

	// Apply changes to temporary blueprints and validate them.
	std::vector<BlueprintPtr> blueprints;
	for (ShipType type : PlayerShipTypesRange())
		blueprints.push_back(BlueprintPtr(new Blueprint(team.GetBlueprint(type))));

	const std::vector<ShipPart> parts = GetParts(team);
	std::set<ShipPart> ancientParts;

	for (auto& c : m.m_changes)
	{
		VERIFY_INPUT(std::find(parts.begin(), parts.end(), c.part) != parts.end());

		if (ShipLayout::IsAncientShipPart(c.part))
			VERIFY_INPUT_MSG("Ancient ship part specified more than once", ancientParts.insert(c.part).second);

		blueprints[(int)c.ship]->SetSlot(c.slot, c.part);
	}

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

//-----------------------------------------------------------------------------
UpgradeDiscoveryCmd::UpgradeDiscoveryCmd(Colour colour, const LiveGame& game, ShipPart part) : UpgradeCmd(colour, game), m_part(part) {}

std::vector<ShipPart> UpgradeDiscoveryCmd::GetParts(const Team& team) const 
{
	return std::vector<ShipPart>(1, m_part); 
}

void UpgradeDiscoveryCmd::Save(Serial::SaveNode& node) const
{
	__super::Save(node);
	node.SaveEnum("part", m_part);
}

void UpgradeDiscoveryCmd::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadEnum("part", m_part);
}

REGISTER_DYNAMIC(UpgradeDiscoveryCmd)
