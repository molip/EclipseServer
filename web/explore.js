var Explore = {}

///////////////////////////////////////////////////////////////////////////////
// Input

Explore.OnCommandChoosePos = function(elem)
{
	ShowActionElement('choose_explore_pos')
	Map.selected = {}
	Map.selecting = true

	data.action = {}
	data.action.positions = []

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
	
	Map.DrawPositions()
}

Explore.OnCommandChooseHex = function(elem)
{
	Map.selecting = false
	ShowActionElement('choose_explore_hex')

	data.action = {}
	data.action.x = Number(elem.getAttribute('x'))
	data.action.y = Number(elem.getAttribute('y'))
	data.action.hexes = [] // id, rotations, rot_idx
	data.action.hex_idx = 0
	
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
	
	Explore.DrawHex()
	Map.DrawSelected(data.action.x, data.action.y)
}

///////////////////////////////////////////////////////////////////////////////
// Output

Explore.SendPos = function()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "cmd_explore_pos")
	node.setAttribute("pos_idx", data.action.pos_idx)
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

Explore.DrawHex = function()
{
	var hex = data.action.hexes[data.action.hex_idx]

	var ctx = Map.layer_action.getContext("2d");
	Map.ClearContext(ctx)
	Map.DrawHex(ctx, hex.id, data.action.x, data.action.y, hex.rotations[hex.rot_idx])
}

Explore.Rotate = function(steps)
{
	var hex = data.action.hexes[data.action.hex_idx]

	hex.rot_idx = (hex.rot_idx + steps) % hex.rotations.length
	if (hex.rot_idx < 0)
		hex.rot_idx += hex.rotations.length

	Explore.DrawHex()
}

Explore.Switch = function()
{
	data.action.hex_idx = (data.action.hex_idx + 1) % data.action.hexes.length
	Explore.DrawHex()
}

