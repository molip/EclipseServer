var Uncolonise = {}

Uncolonise.Stage = function(x, y, num_ships, max_cubes, square_counts)
{
	this.selected = new Point(x, y)
	this.coloniser = new Coloniser(num_ships, max_cubes, square_counts)

	this.Update()
}

Uncolonise.Stage.prototype.OnButton = function(type, delta)
{
	if (delta > 0)
		this.coloniser.Add(type)
	else if (delta < 0)
		this.coloniser.Remove(type)
		
	this.Update()
	Team.UpdatePopulation()
}

Uncolonise.Stage.prototype.Update = function()
{
	this.coloniser.UpdatePlanets()
	
	var c = this.coloniser
	var html = ''

	for (var t in c.planets)
		if (c.planets[t])
			html += '{0}: {1} / {2}<br>'.format(t, c.planets[t] - c.unused_planets[t], c.planets[t])
	
	document.getElementById('choose_uncolonise_content').innerHTML = html

	var total_moved = 0
	for (var t in c.moved)
	{
		total_moved += c.moved[t]
		document.getElementById('choose_uncolonise_add_' + t).disabled = !c.CanAdd(t)
		document.getElementById('choose_uncolonise_remove_' + t).disabled = !c.CanRemove(t)
	}
	document.getElementById('choose_uncolonise_btn').disabled = total_moved == 0
}

Uncolonise.Stage.prototype.Send = function()
{
	var json = CreateCommandJSON('cmd_uncolonise')

	json.moved = ShallowCopy(this.coloniser.moved)
	
	ExitAction()

	SendJSON(json)
}

Uncolonise.Stage.prototype.CleanUp = function(undo)
{
	this.coloniser = null 
	if (undo)
		Team.UpdatePopulation()
}

Uncolonise.Stage.prototype.ModifyPopulation = function(pop)
{
	if (this.coloniser)
		for (t in pop)
			pop[t] += this.coloniser.moved[t]
}

///////////////////////////////////////////////////////////////////////////////
// Input

Uncolonise.OnCommand = function(elem)
{
	ShowActionElement('choose_uncolonise')
	Map.selecting = false

	data.action = new Uncolonise.Stage(elem.x, elem.y, elem.ships, elem.max_cubes, elem.square_counts)
	Map.DrawSelectLayer()
}
