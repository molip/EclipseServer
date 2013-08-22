var Build = {}

Build.Stage = function(ships, hexes) // pos, orbital, monolith
{
	document.getElementById('choose_build_btn').disabled = true

	ClearSelect(document.getElementById('select_build'))
	
	this.ships = ships
	this.hexes = hexes
	this.hex_idx = -1
	this.selected = null

	Map.selecting = true
}

Build.Stage.prototype.OnHexMouseDown = function(pt)
{
	for (var i = 0; i < this.hexes.length; ++i)
		if (this.hexes[i].pos.equals(pt))
		{
			this.selected = pt.Clone()
			document.getElementById('choose_build_btn').disabled = false
			
			var select = document.getElementById('select_build')

			ClearSelect(select)
			
			for (var j = 0; j < this.ships.length; ++j)
				select.add(new Option(this.ships[j], ''))
			
			if (this.hexes[i].orbital)
				select.add(new Option('Orbital', ''))
			if (this.hexes[i].monolith)
				select.add(new Option('Monolith', ''))
			
			this.hex_idx = i
			return true
		}
	return false
}

Build.Stage.prototype.OnDraw = function(ctx)
{
	for (var i = 0; i < this.hexes.length; ++i)
		Map.DrawCentred(ctx, Map.img_explore, this.hexes[i].pos)
}

Build.Stage.prototype.Send = function()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "cmd_build")
	node.setAttribute("x", this.selected.x)
	node.setAttribute("y", this.selected.y)

	var select = document.getElementById('select_build')
	node.setAttribute("buildable", select.options[select.selectedIndex].text)
	
	ExitAction()

	SendXMLDoc(doc)
}

Build.Stage.prototype.CleanUp = function()
{
	Map.ClearCanvas(Map.layer_action)
	Map.ClearCanvas(Map.layer_select)
}

///////////////////////////////////////////////////////////////////////////////
// Input

Build.OnCommand = function(elem)
{
	ShowActionElement('choose_build')

	var can_skip = IsTrue(elem.getAttribute('can_skip'))
	ShowElementById('choose_build_reject_btn', can_skip, true)

	var ships = []
	var ship_elems = GetChildElements(GetFirstChildElement(elem, 'ships'), 'ship')
		for (var i = 0; i < ship_elems.length; ++i)
			ships.push(ship_elems[i].getAttribute('type'))

	var hexes = []
	var hex_elems = GetChildElements(GetFirstChildElement(elem, 'hexes'), 'hex')
	for (var i = 0; i < hex_elems.length; ++i)
	{
		var x = hex_elems[i].getAttribute('x')
		var y = hex_elems[i].getAttribute('y')
		var orb = IsTrue(hex_elems[i].getAttribute('can_build_orbital'))
		var mono = IsTrue(hex_elems[i].getAttribute('can_build_monolith'))
		
		hexes.push( { pos:new Point(x, y), orbital:orb, monolith:mono } )
	}
		
	data.action = new Build.Stage(ships, hexes)

	Map.DrawActionLayer()
	Map.DrawSelectLayer()
}
