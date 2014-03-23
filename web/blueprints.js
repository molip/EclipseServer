var Blueprints = {}
Blueprints.dragPart = null

Blueprints.IsRealPart = function(part_name)
{
	return part_name != 'Empty' && part_name != 'Blocked'
}

Blueprints.GetPartURL = function(part_name)
{
	return Blueprints.IsRealPart(part_name) ? 'images/ship_parts/{0}.png'.format(part_name) : ''
}

Blueprints.SetPart = function(team_id, ship, slot, part_name)
{
	Blueprints[team_id].ships[ship][slot].src = Blueprints.GetPartURL(part_name)
}

//Blueprints.SetOverlay = function(team_id, ship_index, overlay) // overlay[slot_index]
//{
//	for (var j = 0; j < Blueprints[team_id].ships[ship_index].length; ++j)
//		Blueprints.SetPart(team_id, ship_index, j, overlay[j])
//}

Blueprints.CanDrop = function(ev, ship, slot)
{
	var part_name = Blueprints.dragPart
	return data.action.CanDropPart(ship, slot, part_name)
}

Blueprints.OnDrop = function(ev, ship, slot)
{
	var part_name = Blueprints.dragPart
	ev.preventDefault()
	ev.target.src = 'images/ship_parts/{0}.png'.format(part_name)
	ev.target.style.borderStyle = 'none'
	
	data.action.OnDropPart(ship, slot, part_name)
}

Blueprints.EnableDrop = function(team_id, enable)
{
	for (var i = 0; i < 4; ++i)
		for (var j = 0, part; part = Blueprints[team_id].ships[i][j]; ++j)
		{
			var image = Blueprints[team_id].ships[i][j]
			if (enable)
			{
				var f = function (i, j) 
				{
					image.ondragover = function(ev) { if (Blueprints.CanDrop(ev, i, j)) ev.preventDefault() }
					image.ondragenter = function(ev) { if (Blueprints.CanDrop(ev, i, j)) ev.target.style.borderStyle = 'dashed' }
					image.ondragleave = function(ev) { ev.target.style.borderStyle = 'none' }
					image.ondrop = function(ev) { Blueprints.OnDrop(ev, i, j) } 
				}
				f(i, j)
			}
			else
				image.ondrop = image.ondragover = image.ondragenter = image.ondragleave = null
		}
}

Blueprints.dragStart = function(ev, partName)
{
	Blueprints.dragPart = partName
}

Blueprints.Init = function(team_id)
{
	var overlay_id = GetTeamDivIDFromName(team_id, 'blueprints_overlay')
	var div = document.getElementById(overlay_id)

	Blueprints[team_id] = {}
	Blueprints[team_id].ships = {}

	function addImage(ship, x, y)
	{
		var image = new Image()
		image.style.left = '{0}px'.format(x / 2)
		image.style.top = '{0}px'.format(y / 2)
		image.style.position = 'absolute'
		image.width = 52
		image.height = 52
		image.style.borderColor = 'red'
		image.style.borderWidth = '2px'
		
		div.appendChild(image)

		if (Blueprints[team_id].ships == null)
			Blueprints[team_id].ships = []
		if (Blueprints[team_id].ships[ship] == null)
			Blueprints[team_id].ships[ship] = []

		Blueprints[team_id].ships[ship].push(image)
	}
	
	addImage(0, 122, 83)
	addImage(0, 18, 162)
	addImage(0, 122, 186)
	addImage(0, 226, 162)
	
	addImage(1, 490, 20)
	addImage(1, 386, 84)
	addImage(1, 490, 124)
	addImage(1, 594, 84)
	addImage(1, 386, 188)
	addImage(1, 594, 188)

	addImage(2, 36, 319)
	addImage(2, 244, 319)
	addImage(2, 36, 423)
	addImage(2, 140, 389)
	addImage(2, 244, 423)
	addImage(2, 36, 527)
	addImage(2, 140, 492)
	addImage(2, 244, 527)

	addImage(3, 499, 319)
	addImage(3, 396, 424)
	addImage(3, 499, 424)
	addImage(3, 603, 424)
	addImage(3, 499, 527)
}
