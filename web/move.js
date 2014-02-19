var Move = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Move.SrcStage = function(hexes)
{
	document.getElementById('choose_move_src_btn').disabled = true

	ClearSelect(document.getElementById('select_move_ship'))
	
	this.hexes = hexes //pos, ships
	this.hex_idx = -1
	this.selected = null

	Map.selecting = true
}

Move.SrcStage.prototype.OnHexMouseDown = function(pt)
{
	for (var i = 0; i < this.hexes.length; ++i)
		if (pt.equals(this.hexes[i].pos))
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
		Map.DrawCentred(ctx, Map.img_explore, this.hexes[i].pos)
}

Move.SrcStage.prototype.SendPos = function()
{
	var json = CreateCommandJSON('cmd_move_src')
	json.x = this.selected.x
	json.y = this.selected.y

	var ship_idx = document.getElementById('select_move_ship').selectedIndex
	var ship = this.hexes[this.hex_idx].ships[ship_idx]
	json.ship = ship
	
	ExitAction()

	SendJSON(json)
}

Move.SrcStage.prototype.CleanUp = function()
{
	Map.ClearCanvas(Map.layer_action)
	Map.ClearCanvas(Map.layer_select)
}

Move.DstStage = function(hexes)
{
	document.getElementById('choose_move_dst_btn').disabled = true

	this.hexes = hexes // x, y
	this.selected = null

	Map.selecting = true
}

Move.DstStage.prototype.OnHexMouseDown = function(pt)
{
	for (var i = 0; i < this.hexes.length; ++i)
		if (pt.equals(this.hexes[i]))
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
	var json = CreateCommandJSON('cmd_move_dst')
	json.x = this.selected.x
	json.y = this.selected.y
	
	ExitAction()

	SendJSON(json)
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

	ShowElementById('choose_move_src_reject_btn', elem.can_skip, true)

	data.action = new Move.SrcStage(elem.hexes)

	Map.DrawActionLayer()
	Map.DrawSelectLayer()
}

Move.OnCommandChooseDst = function(elem)
{
	ShowActionElement('choose_move_dst')

	data.action = new Move.DstStage(elem.hexes)

	Map.DrawActionLayer()
	Map.DrawSelectLayer()
}
