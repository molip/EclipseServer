var Explore = {}

///////////////////////////////////////////////////////////////////////////////
// Input

Explore.OnCommandChoosePos = function(elem)
{
	ShowActionElement('choose_explore_pos')
	Map.selecting = true

	data.action = {}
	data.action.positions = []
	data.action.OnHexMouseDown = Explore.OnHexMouseDown
	data.action.OnDraw = Explore.OnDrawPositions
	data.action.selected = {}

	var positions = GetChildElements(elem, 'pos')
	for (var i = 0; i < positions.length; ++i)
	{
		var pos = {}
		pos.x = positions[i].getAttribute('x')
		pos.y = positions[i].getAttribute('y')
		data.action.positions.push(pos)
	}
	data.action.pos_idx = -1
	
	var can_skip = IsTrue(elem.getAttribute('can_skip'))
	
	document.getElementById('choose_explore_pos_btn').disabled = true
	ShowElementById('choose_explore_pos_reject_btn', can_skip, true)
	
	Map.DrawActionLayer()
}

Explore.OnCommandChooseHex = function(elem)
{
	Map.selecting = false
	ShowActionElement('choose_explore_hex')

	data.action = {}
	data.action.selected = {}
	data.action.selected.x = Number(elem.getAttribute('x'))
	data.action.selected.y = Number(elem.getAttribute('y'))
	data.action.hexes = [] // id, rotations, rot_idx
	data.action.hex_idx = 0
	data.action.OnDraw = Explore.OnDrawHex
	
	var hexes = GetChildElements(elem, 'hex')
	for (var i = 0; i < hexes.length; ++i)
	{
		var hex = {}
		hex.id = hexes[i].getAttribute('id')
		hex.rot_idx = 0
		hex.rotations = []
		
		var rotations = GetChildElements(hexes[i], 'rotation')
		for (var j = 0; j < rotations.length; ++j)
			hex.rotations.push(rotations[j].getAttribute('steps'))
		
		data.action.hexes.push(hex)
	}
	//alert(hexes.length)

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

Explore.OnHexMouseDown = function(x, y)
{
	if (data.action && data.action.positions)
	{
		for (var i = 0; i < data.action.positions.length; ++i)
			if (data.action.positions[i].x == Map.hot.x && data.action.positions[i].y == Map.hot.y)
			{
				data.action.selected.x = Map.hot.x
				data.action.selected.y = Map.hot.y
				data.action.pos_idx = i
				document.getElementById('choose_explore_pos_btn').disabled = false
				return true
			}
	}
	return false
}

Explore.OnDrawPositions = function(ctx)
{
	Assert(data.action.positions, "Explore.OnDrawPositions")

	for (var i = 0; i < data.action.positions.length; ++i)
		Map.DrawCentred(ctx, Map.img_explore, data.action.positions[i].x, data.action.positions[i].y)
}

Explore.OnDrawHex = function(ctx)
{
	Assert(data.action.selected && data.action.hexes, "Explore.OnDrawHex")

	var hex = data.action.hexes[data.action.hex_idx]
	Map.DrawHex(ctx, new Map.Hex(hex.id, data.action.selected.x, data.action.selected.y, hex.rotations[hex.rot_idx]))
}
