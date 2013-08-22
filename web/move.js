var Move = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Move.SrcStage = function()
{
	document.getElementById('choose_move_src_btn').disabled = true

	ClearSelect(document.getElementById('select_move_ship'))
	
	this.hexes = [] //pt, ships
	this.hex_idx = -1
	this.selected = null

	Map.selecting = true
}

Move.SrcStage.prototype.AddHex = function(pt, ships)
{
	this.hexes.push( { pt:pt, ships:ships } )
}

Move.SrcStage.prototype.OnHexMouseDown = function(pt)
{
	for (var i = 0; i < this.hexes.length; ++i)
		if (this.hexes[i].pt.equals(pt))
		{
			this.selected = pt.Clone()
			document.getElementById('choose_move_src_btn').disabled = false
			
			var select = document.getElementById('select_move_ship')

			ClearSelect(select)
			
			var ships = this.hexes[i].ships
			for (var j = 0; j < ships.length; ++j)
				select.add(new Option(ships[j], ''))

			this.hex_idx = i
			return true
		}
	return false
}

Move.SrcStage.prototype.OnDraw = function(ctx)
{
	for (var i = 0; i < this.hexes.length; ++i)
		Map.DrawCentred(ctx, Map.img_explore, this.hexes[i].pt)
}

Move.SrcStage.prototype.SendPos = function()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "cmd_move_src")
	node.setAttribute("x", this.selected.x)
	node.setAttribute("y", this.selected.y)

	var ship_idx = document.getElementById('select_move_ship').selectedIndex
	var ship = this.hexes[this.hex_idx].ships[ship_idx]
	node.setAttribute("ship", ship)
	
	ExitAction()

	SendXMLDoc(doc)
}

Move.SrcStage.prototype.CleanUp = function()
{
	Map.ClearCanvas(Map.layer_action)
	Map.ClearCanvas(Map.layer_select)
}

Move.DstStage = function(hexes)
{
	document.getElementById('choose_move_dst_btn').disabled = true

	this.hexes = hexes
	this.selected = null

	Map.selecting = true
}

Move.DstStage.prototype.OnHexMouseDown = function(pt)
{
	for (var i = 0; i < this.hexes.length; ++i)
		if (this.hexes[i].equals(pt))
		{
			this.selected = pt.Clone()
			document.getElementById('choose_move_dst_btn').disabled = false
			return true
		}
	return false
}

Move.DstStage.prototype.OnDraw = function(ctx)
{
	for (var i = 0; i < this.hexes.length; ++i)
		Map.DrawCentred(ctx, Map.img_explore, this.hexes[i])
}

Move.DstStage.prototype.SendPos = function()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "cmd_move_dst")
	node.setAttribute("x", this.selected.x)
	node.setAttribute("y", this.selected.y)
	
	ExitAction()

	SendXMLDoc(doc)
}

Move.DstStage.prototype.CleanUp = function()
{
	Map.ClearCanvas(Map.layer_action)
	Map.ClearCanvas(Map.layer_select)
}

///////////////////////////////////////////////////////////////////////////////
// Input

Move.OnCommandChooseSrc = function(elem)
{
	ShowActionElement('choose_move_src')

	var can_skip = IsTrue(elem.getAttribute('can_skip'))
	ShowElementById('choose_move_src_reject_btn', can_skip, true)

	data.action = new Move.SrcStage()

	var hexes = GetChildElements(elem, 'hex')
	for (var i = 0; i < hexes.length; ++i)
	{
		var x = hexes[i].getAttribute('x')
		var y = hexes[i].getAttribute('y')
		var ships = []
	
		var ship_elems = GetChildElements(hexes[i], 'ship')
		for (var j = 0; j < ship_elems.length; ++j)
			ships.push(ship_elems[j].getAttribute('type'))
		
		data.action.AddHex(new Point(x, y), ships)
	}

	Map.DrawActionLayer()
	Map.DrawSelectLayer()
}

Move.OnCommandChooseDst = function(elem)
{
	ShowActionElement('choose_move_dst')

	var hexes = []
	var hex_elems = GetChildElements(elem, 'hex')
	for (var i = 0; i < hex_elems.length; ++i)
	{
		var x = hex_elems[i].getAttribute('x')
		var y = hex_elems[i].getAttribute('y')
		
		hexes.push(new Point(x, y))
	}

	data.action = new Move.DstStage(hexes)

	Map.DrawActionLayer()
	Map.DrawSelectLayer()
}
