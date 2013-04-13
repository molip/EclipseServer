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
		if (this.positions[i].equals(pt))
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
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "cmd_explore_pos")
	node.setAttribute("pos_idx", this.pos_idx)

	ExitAction()

	SendXMLDoc(doc)
}

Explore.ChoosePosStage.prototype.CleanUp = function()
{
	Map.ClearCanvas(Map.layer_action)
	Map.ClearCanvas(Map.layer_select)
}

///////////////////////

Explore.ChooseHexStage = function(pos)
{
	this.selected = pos.Clone()
	this.hexes = [] // id, rotations, rot_idx, can_influence
	this.hex_idx = 0
}

Explore.ChooseHexStage.prototype.AddHex = function(id, rotations, can_influence)
{
	var hex = { id:id, rot_idx:0, rotations:rotations, can_influence:can_influence }
	this.hexes.push(hex)
}

Explore.ChooseHexStage.prototype.OnDraw = function(ctx)
{
	var hex = this.hexes[this.hex_idx]
	Map.DrawHex(ctx, new Map.Hex(hex.id, this.selected, hex.rotations[hex.rot_idx]))
}

Explore.ChooseHexStage.prototype.SendHex = function()
{
	var hex = this.hexes[this.hex_idx]
	var influence = hex.can_influence && document.getElementById('choose_explore_hex_influence_check').checked 
	
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "cmd_explore_hex")
	node.setAttribute("rot_idx", hex.rot_idx)
	node.setAttribute("hex_idx", this.hex_idx)
	node.setAttribute("influence", influence)

	ExitAction()
	
	SendXMLDoc(doc)
}

Explore.ChooseHexStage.prototype.CleanUp = function()
{
	Map.ClearCanvas(Map.layer_action)
	Map.ClearCanvas(Map.layer_select)
}

Explore.ChooseHexStage.prototype.Rotate = function(steps)
{
	var hex = this.hexes[this.hex_idx]

	hex.rot_idx = (hex.rot_idx + steps) % hex.rotations.length
	if (hex.rot_idx < 0)
		hex.rot_idx += hex.rotations.length

	Map.DrawActionLayer()
}

Explore.ChooseHexStage.prototype.Switch = function()
{
	this.hex_idx = (this.hex_idx + 1) % this.hexes.length
	this.UpdateInfluenceCheckbox()
	Map.DrawActionLayer()
}

Explore.ChooseHexStage.prototype.UpdateInfluenceCheckbox = function()
{
	var cb = document.getElementById('choose_explore_hex_influence_check')
	var hex = this.hexes[this.hex_idx]

	cb.checked = hex.can_influence
	cb.disabled = !hex.can_influence 
}
	///////////////////////////////////////////////////////////////////////////////
// Input

Explore.OnCommandChoosePos = function(elem)
{
	ShowActionElement('choose_explore_pos')
	Map.selecting = true

	var positions = []
	var pos_elems = GetChildElements(elem, 'pos')
	for (var i = 0; i < pos_elems.length; ++i)
	{
		var x = pos_elems[i].getAttribute('x')
		var y = pos_elems[i].getAttribute('y')
		positions.push(new Point(x, y))
	}

	data.action = new Explore.ChoosePosStage(positions)
	
	var can_skip = IsTrue(elem.getAttribute('can_skip'))
	var can_undo = IsTrue(elem.getAttribute('can_undo'))
	
	document.getElementById('choose_explore_pos_btn').disabled = true
	ShowElementById('choose_explore_pos_reject_btn', can_skip, true)

	ShowElementById('choose_undo', true)
	document.getElementById('choose_undo_btn').disabled = !can_undo
	
	Map.DrawActionLayer()
}

Explore.OnCommandChooseHex = function(elem)
{
	Map.selecting = false
	ShowActionElement('choose_explore_hex')

	var x = Number(elem.getAttribute('x'))
	var y = Number(elem.getAttribute('y'))

	data.action = new Explore.ChooseHexStage(new Point(x, y))
	
	var hexes = GetChildElements(elem, 'hex')
	for (var i = 0; i < hexes.length; ++i)
	{
		var id = hexes[i].getAttribute('id')
		var can_influence = IsTrue(hexes[i].getAttribute('can_influence'))
		var rotations = []
		
		var rot_elems = GetChildElements(hexes[i], 'rotation')
		for (var j = 0; j < rot_elems.length; ++j)
			rotations.push(rot_elems[j].getAttribute('steps'))
		
		data.action.AddHex(id, rotations, can_influence)
	}

	ShowElementById('choose_explore_hex_switch_btn', hexes.length > 1, true)
	
	data.action.UpdateInfluenceCheckbox()

	var can_undo = IsTrue(elem.getAttribute('can_undo'))
	ShowElementById('choose_undo', true)
	document.getElementById('choose_undo_btn').disabled = !can_undo
	
	Map.DrawActionLayer()
	Map.DrawSelectLayer()
}

Explore.SendReject = function()
{
	ExitAction()

	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "cmd_explore_reject")
	SendXMLDoc(doc)
}
