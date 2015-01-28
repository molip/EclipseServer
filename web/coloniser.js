 var Coloniser = function(ship_count, max_moves, planets)
{
	this.ship_count = ship_count
	this.max_moves = max_moves
	this.planets = planets
	
	this.unused_planets = ShallowCopy(planets)
	this.moved = {}
		
	for (var t in max_moves)
		 this.moved[t] = 0

	this.planet_count = 0
	for (var p in this.planets)
		this.planet_count += planets[p]
}

Coloniser.prototype.CanAdd = function(type)
{
	if (this.ship_count <= 0 || this.moved[type] >= this.max_moves[type])
		return false;
	
	for (var p in this.planets)
		if (this.unused_planets[p] && this.IsTypeCompatibleWithPlanet(type, p))
			return true;
	return false
}

Coloniser.prototype.CanRemove = function(type)
{
	return this.moved[type] > 0
}
	
Coloniser.prototype.IsTypeCompatibleWithPlanet = function(type, planet)
{
	return 	type == planet || 
			planet == 'Any' || 
			(type == 'Money' || type == 'Science') && planet == 'Orbital'
}
	
Coloniser.prototype.Add = function(type)
{
	Assert(this.CanAdd(type))
	++this.moved[type]
	--this.ship_count
}
	
Coloniser.prototype.Remove = function(type)
{
	Assert(this.CanRemove(type))
	--this.moved[type]
	++this.ship_count
}

Coloniser.prototype.UpdatePlanets = function()
{
	var m = ShallowCopy(this.moved)
	this.unused_planets = ShallowCopy(this.planets)
	
	for (var p in this.planets)
	{
		for (var type in m)
			if (this.IsTypeCompatibleWithPlanet(type, p))
			{
				var move = Math.min(m[type], this.unused_planets[p])
				m[type] -= move
				this.unused_planets[p] -= move
			}
	}
	for (var type in m)
		Assert(m[type] == 0)
}