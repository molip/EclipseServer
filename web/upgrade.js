var Upgrade = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Upgrade.Stage = function(elem)
{
	Assert(elem.blueprints.length == 4)
	
	// blueprints[ship_index] { fixed_power, base_layout[slot_index] { name, power_source, power_drain, is_drive }, overlay[slot_index] }
	this.blueprints = elem.blueprints 
	
	this.changes = [] // { ship, slot, part }
	this.added = 0
	this.max_upgrades = elem.max_upgrades
	
	var div = document.getElementById('choose_upgrade_parts')
	for (var i = 0, part; part = elem.parts[i]; ++i)
	{
		var partName = part.name
		var image = new Image()
		image.src = Blueprints.GetPartURL(partName)
		image.height = 52
		image.draggable = true
		image.title = part.name
		
		var f = function(part) { image.ondragstart = function(event) { Blueprints.dragStart(event, part.name) } } 
		f(part)

		div.appendChild(image)
	}

	data.teams[data.playerID].can_edit_blueprints = true
		
	if (IsCurrentTeam(data.playerID))
		Team.UpdateEditBlueprints()
	
	ShowTeamBlueprints()
	
	this.UpdateRemainingUI();
}


Upgrade.Stage.prototype.UpdateRemainingUI = function()
{
	document.getElementById('choose_upgrade_remaining').innerHTML = this.max_upgrades - this.added
}

Upgrade.Stage.prototype.OnDropPart = function(ship, slot, part_name)
{
	this.changes.push( { ship:ship, slot:slot, part:part_name } )
	++this.added
	this.UpdateRemainingUI();
}

Upgrade.Stage.prototype.CanDropPart = function(ship, slot, part_name)
{
	var base_part = this.blueprints[ship].base_layout[slot].name
	var overlay_part = this.blueprints[ship].overlay[slot].name

	for (var i = 0, change; change = this.changes[i]; ++i)
		if (change.ship == ship && change.slot == slot && change.part == part_name)
			return false;
	
	if (this.added >= this.max_upgrades)
		return false
	
	return base_part != 'Blocked' && part_name != overlay_part && part_name != base_part
}

Upgrade.Stage.prototype.Send = function()
{
	var json = CreateCommandJSON('cmd_upgrade')

	json.changes = this.changes
	
	ExitAction()

	SendJSON(json)
}

Upgrade.Stage.prototype.CleanUp = function()
{
	ShowTeamGeneral()

	var div = document.getElementById('choose_upgrade_parts')
	while (div.firstChild)
		div.removeChild(div.firstChild)

	data.teams[data.playerID].can_edit_blueprints = false

	if (IsCurrentTeam(data.playerID))
	{
		Team.UpdateEditBlueprints()
		Team.UpdateBlueprints()
	}
}

///////////////////////////////////////////////////////////////////////////////
// Input

Upgrade.OnCommand = function(elem)
{
	ShowActionElement('choose_upgrade')
	data.action = new Upgrade.Stage(elem)
}
