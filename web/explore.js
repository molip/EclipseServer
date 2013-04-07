///////////////////////////////////////////////////////////////////////////////
// Input

function OnCommandChooseExplorePos(elem)
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
	
	var can_skip = IsTrue(elem.getAttribute('can_skip'))
	
	document.getElementById('choose_explore_pos_btn').disabled = true
	ShowElementById('choose_explore_pos_reject_btn', can_skip, true)
	
	DrawPositions()
}

function OnCommandChooseExploreHex(elem)
{
	Map.selecting = false
	ShowActionElement('choose_explore_hex')

	data.action = {}
	data.action.x = Number(elem.getAttribute('x'))
	data.action.y = Number(elem.getAttribute('y'))
	data.action.hexes = []
	data.action.hex_idx = 0
	data.action.rotation = 0
	
	var hexes = GetChildElements(elem, 'hex')
	for (var i = 0; i < hexes.length; ++i)
	{
		var id = hexes[i].getAttribute('id')
		data.action.hexes.push(id)
	}

	ShowElementById('choose_explore_hex_switch_btn', hexes.length > 1, true)
	
	DrawExploreHex()
	DrawSelected(data.action.x, data.action.y)
}

///////////////////////////////////////////////////////////////////////////////
// Output

function SendExplorePos()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "cmd_explore_pos")
	node.setAttribute("x", Map.selected.x)
	node.setAttribute("y", Map.selected.y)
	SendXMLDoc(doc)
}

function SendExploreHex()
{
	ClearCanvas(Map.layer_action)
	ClearCanvas(Map.layer_select)

	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "cmd_explore_hex")
	node.setAttribute("rotation", data.action.rotation)
	node.setAttribute("hex_idx", data.action.hex_idx)

	data.action = null

	SendXMLDoc(doc)
}

function SendExploreReject()
{
	ClearCanvas(Map.layer_action)
	ClearCanvas(Map.layer_select)
	data.action = null

	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "cmd_explore_reject")
	SendXMLDoc(doc)
}

///////////////////////////////////////////////////////////////////////////////
// UI

function DrawExploreHex()
{
	var id = data.action.hexes[data.action.hex_idx]
	var ctx = Map.layer_action.getContext("2d");
	ClearContext(ctx)
	DrawHex(ctx, id, data.action.x, data.action.y, data.action.rotation)
}

function ExploreRotate(steps)
{
	data.action.rotation = (data.action.rotation + steps) % 6
	if (data.action.rotation < 0)
		data.action.rotation += 6

	DrawExploreHex()
}

function ExploreSwitch()
{
	data.action.hex_idx = (data.action.hex_idx + 1) % data.action.hexes.length
	DrawExploreHex()
}

