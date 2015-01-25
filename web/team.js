var Team = {}

Team.SetDivHTML = function(elem_suffix, html)
{
	document.getElementById('team_' + elem_suffix).innerHTML = html
}

Team.UpdateAll = function()
{
	Team.UpdateSummary()
	Team.UpdatePassed()
	Team.UpdateInfluence()
	Team.UpdateActions()
	Team.UpdateColonyShips()
	Team.UpdateStorage()
	Team.UpdatePopulation()
	Team.UpdateTechnology()
	Team.UpdateReputation()
	Team.UpdateBlueprints()
	
	Team.UpdateInfluenceSelection()
	Team.UpdateEditBlueprints()
}

Team.UpdateSummary = function()
{
	var team = GetCurrentTeam()
	var html = '<b>Team:</b> {0}, <b>Race:</b> {1}, <b>Colour:</b> {2}<br/><br/>'.format(team.name, team.race, team.colour)
	Team.SetDivHTML('summary', html)
}

Team.UpdatePassed = function()
{
	var team = GetCurrentTeam()
	var html = '<b>Passed:</b> {0}<br/>'.format(team.has_passed)
	Team.SetDivHTML('passed', html)
}

Team.UpdateInfluence = function()
{
	var team = GetCurrentTeam()
	var html = '<b>Influence discs:</b> {0} (upkeep -{1})<br/>'.format(team.discs, team.upkeep)
	Team.SetDivHTML('influence', html)
}

Team.UpdateActions = function()
{
	var team = GetCurrentTeam()
	var html = '<b>Actions done:</b> {0}<br/>'.format(team.actions)
	Team.SetDivHTML('actions', html)
}

Team.UpdateColonyShips = function()
{
	var team = GetCurrentTeam()
	var html = '<b>Colony ships left:</b> {0}/{1}<br/>'.format(team.colony_ships_used, team.colony_ships_total)
	Team.SetDivHTML('colony_ships', html)
}

Team.UpdateStorage = function()
{
	var team = GetCurrentTeam()
	var html = '<b>Storage:</b>Money: {0} Science: {1} Materials:{2}<br/>'.format(team.Money, team.Science, team.Materials)
	Team.SetDivHTML('storage', html)
}

Team.UpdatePopulation = function()
{
	var team = GetCurrentTeam()
	var pop = ShallowCopy(team.population)

	if (IsCurrentTeam(data.playerID) && data.action && typeof(data.action.ModifyPopulation) === 'function')
		data.action.ModifyPopulation(pop)
	
	Population.Draw(pop, team.colour)
}

Team.UpdateTechnology = function()
{
	var team = GetCurrentTeam()
	var html = '<b>Technology:</b><br/>'
	
	for (var i = 0, cls; cls = team.classes[i]; ++i)
	{
		html += cls.name + ':'
		for (var j = 0, tech; tech = cls.techs[j]; ++j)
			html += tech.name + ','
		html += '<br/>'
	}
	Team.SetDivHTML('technology', html)
}

Team.UpdateReputation = function()
{
	var team = GetCurrentTeam()
	var html = '<b>Reputation:</b>Tiles: {0}/{1}<br/>'.format(team.tiles, team.slots)
	Team.SetDivHTML('reputation', html)
}

Team.UpdateVictoryTiles = function()
{
	var team = GetCurrentTeam()
	var html = '<b>Victory point tiles:</b> {0}<br/>'.format(team.victory_tiles)
	Team.SetDivHTML('victory_tiles', html)
}

Team.UpdateBlueprints = function()
{
	var team = GetCurrentTeam()

	document.getElementById('team_blueprints_img').src = 'images/blueprints/{0}.png'.format(team.blueprint_type)
	
	Assert(team.blueprints.length == 4)

	for (var ship = 0; ship < 4; ++ship)
		for (var slot = 0, part; part = team.blueprints[ship][slot]; ++slot)
			Blueprints.SetPart(ship, slot, part)
}

Team.UpdateInfluenceSelection = function()
{
	var team = GetCurrentTeam()

	var style = ""
	
	if (team.can_select_influence_track)
		style = team.influence_track_selected ? "2px solid red" : "2px solid green"
		
	document.getElementById('team_influence').style.border = style
}

Team.UpdateEditBlueprints = function()
{
	var team = GetCurrentTeam()
	Blueprints.EnableDrop(team.can_edit_blueprints)
}
