var Explore = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Explore.ChoosePosStage = function(positions)
{
	this.positions = positions
	this.selected = null
	this.pos_idx = -1
}

Explore.ChoosePosStage.prototype.OnHexMouseDown = function(pt)
{
	for (var i = 0; i < this.positions.length; ++i)
		if (pt.equals(this.positions[i]))
		{
			this.selected = pt.Clone()
			this.pos_idx = i
			document.getElementById('choose_explore_pos_btn').disabled = false
			return true
		}
	return false
}

Explore.ChoosePosStage.prototype.OnDraw = function(ctx)
{
	for (var i = 0; i < this.positions.length; ++i)
		Map.DrawCentred(ctx, Map.img_explore, this.positions[i])
}

Explore.ChoosePosStage.prototype.SendPos = function()
{
	var json = CreateCommandJSON("cmd_explore_pos")
	json.pos_idx = this.pos_idx

	ExitAction()

	SendJSON(json)
}

Explore.ChoosePosStage.prototype.CleanUp = function()
{
	Map.ClearCanvas(Map.layer_action)
	Map.ClearCanvas(Map.layer_select)
}

///////////////////////////////////////////////////////////////////////////////

Explore.ChooseHexStage = function(pos, canUndo, hexes)
{
	this.flagCantUndo = !canUndo

	this.selected = pos.Clone()
	this.choices = hexes // id, rotations, can_influence
	this.choice_idx = 0
	
	for (var i = 0, hex; hex = this.choices[i]; ++i)
		hex.rot_idx = 0
}

Explore.ChooseHexStage.prototype.UpdateHex = function()
{
	var choice = this.choices[this.choice_idx]
	this.hex = new Map.Hex(choice.id, this.selected, choice.rotations[choice.rot_idx], null, null, null, Map.DrawActionLayer)
	Map.DrawActionLayer()
}

Explore.ChooseHexStage.prototype.OnDraw = function(ctx)
{
	if (this.hex)
		Map.DrawHex(ctx, this.hex)
}

Explore.ChooseHexStage.prototype.SendHex = function()
{
	var choice = this.choices[this.choice_idx]
	var influence = choice.can_influence && document.getElementById('choose_explore_hex_influence_check').checked 
	
	var json = CreateCommandJSON("cmd_explore_hex")
	json.rot_idx = choice.rot_idx
	json.hex_idx = this.choice_idx
	json.influence = influence

	ExitAction()
	
	SendJSON(json)
}

Explore.ChooseHexStage.prototype.SendTake = function()
{
	var json = CreateCommandJSON('cmd_explore_hex_take')

	ExitAction()

	SendJSON(json)
}

Explore.ChooseHexStage.prototype.CleanUp = function()
{
	Map.ClearCanvas(Map.layer_action)
	Map.ClearCanvas(Map.layer_select)
}

Explore.ChooseHexStage.prototype.Rotate = function(steps)
{
	var choice = this.choices[this.choice_idx]

	choice.rot_idx = (choice.rot_idx + steps) % choice.rotations.length
	if (choice.rot_idx < 0)
		choice.rot_idx += choice.rotations.length

	this.UpdateHex()
}

Explore.ChooseHexStage.prototype.Switch = function()
{
	this.choice_idx = (this.choice_idx + 1) % this.choices.length
	this.UpdateInfluenceCheckbox()

	this.UpdateHex()
}

Explore.ChooseHexStage.prototype.UpdateInfluenceCheckbox = function()
{
	var cb = document.getElementById('choose_explore_hex_influence_check')
	var choice = this.choices[this.choice_idx]

	cb.checked = choice.can_influence
	cb.disabled = !choice.can_influence 
}

///////////////////////////////////////////////////////////////////////////////
// Input

Explore.OnCommandChoosePos = function(elem)
{
	ShowActionElement('choose_explore_pos')
	Map.selecting = true

	data.action = new Explore.ChoosePosStage(elem.positions)
	
	document.getElementById('choose_explore_pos_btn').disabled = true
	ShowElementById('choose_explore_pos_reject_btn', elem.can_skip)

	Map.DrawActionLayer()
}

Explore.OnCommandChooseHex = function(elem)
{
	Map.selecting = false
	ShowActionElement('choose_explore_hex')

	data.action = new Explore.ChooseHexStage(new Point(elem.x, elem.y), elem.can_undo, elem.hexes)

	ShowElementById('choose_explore_hex_switch_btn', elem.hexes.length > 1)
	ShowElementById('choose_explore_hex_take_btn', elem.can_take)
	
	data.action.UpdateInfluenceCheckbox()
	data.action.UpdateHex()

	Map.DrawSelectLayer()
}
