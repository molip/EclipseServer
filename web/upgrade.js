var Upgrade = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Upgrade.Stage = function(elem)
{
	Assert(elem.blueprints.length == 4)
	
	// blueprints[ship_index] { fixed_power, base_layout[slot_index] { name, power_source, power_drain, is_drive }, overlay[slot_index] }
	this.blueprints = elem.blueprints 
	
	this.changes = [] // { ship, slot, name }
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

	Blueprints.EnableDrop(data.playerID, true)
	
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

Upgrade.Stage.prototype.CanDropOn = function(ship, slot)
{
	return this.added < this.max_upgrades && this.blueprints[ship].base_layout[slot].name != 'Blocked'
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

	Blueprints.EnableDrop(data.playerID, false)
	
	for (var ship = 0; ship < 4; ++ship)
		for (var slot = 0, part; part = this.blueprints[ship].overlay[slot]; ++slot)
			Blueprints.SetPart(data.playerID, ship, slot, part.name)
}

///////////////////////////////////////////////////////////////////////////////
// Input

Upgrade.OnCommand = function(elem)
{
	ShowActionElement('choose_upgrade')
	data.action = new Upgrade.Stage(elem)
}
