var Colonise = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Colonise.ChoosePosStage = function(positions)
{
	this.flagNoSubAction = true;

	this.positions = positions
	this.selected = null
	this.pos_idx = -1

	document.getElementById('choose_colonise_pos_btn').disabled = true
}

Colonise.ChoosePosStage.prototype.OnHexMouseDown = function(pt)
{
	for (var i = 0; i < this.positions.length; ++i)
		if (pt.equals(this.positions[i]))
		{
			this.selected = pt.Clone()
			this.pos_idx = i
			document.getElementById('choose_colonise_pos_btn').disabled = false
			return true
		}
	return false
}

Colonise.ChoosePosStage.prototype.OnDraw = function(ctx)
{
	for (var i = 0; i < this.positions.length; ++i)
		Map.DrawCentred(ctx, Map.img_explore, this.positions[i])
}

Colonise.ChoosePosStage.prototype.Send = function()
{
	var json = CreateCommandJSON('cmd_colonise_pos')
	json.pos_idx = this.pos_idx

	ExitAction()

	SendJSON(json)
}

Colonise.ChoosePosStage.prototype.CleanUp = function()
{
	Map.ClearCanvas(Map.layer_select)
	Map.ClearCanvas(Map.layer_action)
}

/////////////

Colonise.ChooseSquaresStage = function(x, y, num_ships, max_cubes, square_counts)
{
	this.selected = new Point(x, y)
	this.coloniser = new Coloniser(num_ships, max_cubes, square_counts)

	this.Update()
}

Colonise.ChooseSquaresStage.prototype.OnButton = function(type, delta)
{
	if (delta > 0)
		this.coloniser.Add(type)
	else if (delta < 0)
		this.coloniser.Remove(type)
		
	this.Update()
	Team.UpdatePopulation()
}

Colonise.ChooseSquaresStage.prototype.Update = function()
{
	this.coloniser.UpdatePlanets()
	
	var c = this.coloniser
	var html = ''

	for (var t in c.planets)
		if (c.planets[t])
			html += '{0}: {1} / {2}<br>'.format(t, c.planets[t] - c.unused_planets[t], c.planets[t])
	
	document.getElementById('choose_colonise_squares_content').innerHTML = html

	var total_moved = 0
	for (var t in c.moved)
	{
		total_moved += c.moved[t]
		document.getElementById('choose_colonise_squares_add_' + t).disabled = !c.CanAdd(t)
		document.getElementById('choose_colonise_squares_remove_' + t).disabled = !c.CanRemove(t)
	}
	document.getElementById('choose_colonise_squares_btn').disabled = total_moved == 0
}

Colonise.ChooseSquaresStage.prototype.Send = function()
{
	var json = CreateCommandJSON('cmd_colonise_squares')

	json.moved = ShallowCopy(this.coloniser.moved)
	
	ExitAction()

	SendJSON(json)
}

Colonise.ChooseSquaresStage.prototype.CleanUp = function(undo)
{
	this.coloniser = null 
	if (undo)
		Team.UpdatePopulation()
}

Colonise.ChooseSquaresStage.prototype.ModifyPopulation = function(pop)
{
	if (this.coloniser)
		for (t in pop)
			pop[t] -= this.coloniser.moved[t]
}

///////////////////////////////////////////////////////////////////////////////
// Input

Colonise.OnCommandChoosePos = function(elem)
{
	ShowActionElement('choose_colonise_pos')
	Map.selecting = true

	data.action = new Colonise.ChoosePosStage(elem.positions)
	Map.DrawActionLayer()
}

Colonise.OnCommandChooseSquares = function(elem)
{
	ShowActionElement('choose_colonise_squares')
	Map.selecting = false

	data.action = new Colonise.ChooseSquaresStage(elem.x, elem.y, elem.ships, elem.max_cubes, elem.square_counts)
	Map.DrawSelectLayer()
}
