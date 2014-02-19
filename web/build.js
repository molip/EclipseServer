var Build = {}

Build.Stage = function(ships, hexes) 
{
	document.getElementById('choose_build_btn').disabled = true

	ClearSelect(document.getElementById('select_build'))
	
	this.ships = ships
	this.hexes = hexes // pos, can_build_orbital, can_build_monolith
	this.hex_idx = -1
	this.selected = null

	Map.selecting = true
}

Build.Stage.prototype.OnHexMouseDown = function(pt)
{
	for (var i = 0; i < this.hexes.length; ++i)
		if (pt.equals(this.hexes[i].pos))
		{
			this.selected = pt.Clone()
			document.getElementById('choose_build_btn').disabled = false
			
			var select = document.getElementById('select_build')

			ClearSelect(select)
			
			for (var j = 0; j < this.ships.length; ++j)
				select.add(new Option(this.ships[j], ''))
			
			if (this.hexes[i].can_build_orbital)
				select.add(new Option('Orbital', ''))
			if (this.hexes[i].can_build_monolith)
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

	ShowElementById('choose_build_reject_btn', elem.can_skip, true)
		
	data.action = new Build.Stage(elem.ships, elem.hexes)

	Map.DrawActionLayer()
	Map.DrawSelectLayer()
}
