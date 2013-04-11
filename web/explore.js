var Explore = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Explore.ChoosePosStage = function(positions)
{
	this.positions = positions
	this.selected = {}
	this.pos_idx = -1
}

Explore.ChoosePosStage.prototype.OnHexMouseDown = function(x, y)
{
	for (var i = 0; i < this.positions.length; ++i)
		if (this.positions[i].x == x && this.positions[i].y == y)
		{
			this.selected.x = x
			this.selected.y = y
			this.pos_idx = i
			document.getElementById('choose_explore_pos_btn').disabled = false
			return true
		}
	return false
}

Explore.ChoosePosStage.prototype.OnDraw = function(ctx)
{
	for (var i = 0; i < this.positions.length; ++i)
		Map.DrawCentred(ctx, Map.img_explore, this.positions[i].x, this.positions[i].y)
}

///////////////////////

Explore.ChooseHexStage = function(x, y)
{
	this.selected = { x:x, y:y }
	this.hexes = [] // id, rotations, rot_idx
	this.hex_idx = 0
}

Explore.ChooseHexStage.prototype.AddHex = function(id, rotations)
{
	var hex = { id:id, rot_idx:0, rotations:rotations }
	this.hexes.push(hex)
}

Explore.ChooseHexStage.prototype.OnDraw = function(ctx)
{
	var hex = this.hexes[this.hex_idx]
	Map.DrawHex(ctx, new Map.Hex(hex.id, this.selected.x, this.selected.y, hex.rotations[hex.rot_idx]))
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
		var pos = { x:x, y:y }
		positions.push(pos)
	}

	data.action = new Explore.ChoosePosStage(positions)
	
	var can_skip = IsTrue(elem.getAttribute('can_skip'))
	
	document.getElementById('choose_explore_pos_btn').disabled = true
	ShowElementById('choose_explore_pos_reject_btn', can_skip, true)
	
	Map.DrawActionLayer()
}

Explore.OnCommandChooseHex = function(elem)
{
	Map.selecting = false
	ShowActionElement('choose_explore_hex')

	var x = Number(elem.getAttribute('x'))
	var y = Number(elem.getAttribute('y'))

	data.action = new Explore.ChooseHexStage(x, y)
	
	var hexes = GetChildElements(elem, 'hex')
	for (var i = 0; i < hexes.length; ++i)
	{
		var id = hexes[i].getAttribute('id')
		var rotations = []
		
		var rot_elems = GetChildElements(hexes[i], 'rotation')
		for (var j = 0; j < rot_elems.length; ++j)
			rotations.push(rot_elems[j].getAttribute('steps'))
		
		data.action.AddHex(id, rotations)
	}

	ShowElementById('choose_explore_hex_switch_btn', hexes.length > 1, true)
	
	Map.DrawActionLayer()
	Map.DrawSelectLayer()
}

///////////////////////////////////////////////////////////////////////////////
// Output

Explore.SendPos = function()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "cmd_explore_pos")
	node.setAttribute("pos_idx", data.action.pos_idx)

	data.action = null

	SendXMLDoc(doc)
}

Explore.SendHex = function()
{
	Map.ClearCanvas(Map.layer_action)
	Map.ClearCanvas(Map.layer_select)

	var hex = data.action.hexes[data.action.hex_idx]
	
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "cmd_explore_hex")
	node.setAttribute("rot_idx", hex.rot_idx)
	node.setAttribute("hex_idx", data.action.hex_idx)

	data.action = null

	SendXMLDoc(doc)
}

Explore.SendReject = function()
{
	Map.ClearCanvas(Map.layer_action)
	Map.ClearCanvas(Map.layer_select)
	data.action = null
	
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "cmd_explore_reject")
	SendXMLDoc(doc)
}

///////////////////////////////////////////////////////////////////////////////
// UI

Explore.Rotate = function(steps)
{
	var hex = data.action.hexes[data.action.hex_idx]

	hex.rot_idx = (hex.rot_idx + steps) % hex.rotations.length
	if (hex.rot_idx < 0)
		hex.rot_idx += hex.rotations.length

	Map.DrawActionLayer()
}

Explore.Switch = function()
{
	data.action.hex_idx = (data.action.hex_idx + 1) % data.action.hexes.length
	Map.DrawActionLayer()
}
