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

Colonise.ChooseSquaresStage = function(num_ships, square_counts, max_cubes)
{
	this.flagNoSubAction = true;

	this.num_ships = num_ships

	this.max_fixed = [square_counts.Money, square_counts.Science, square_counts.Materials]
	this.max_cubes = [max_cubes.Money, max_cubes.Science, max_cubes.Materials]
	this.max_grey = square_counts.Any
	this.max_orbital = square_counts.Orbital

	document.getElementById('choose_colonise_squares_btn').disabled = true

	var div = document.getElementById('choose_colonise_squares_content')

	html = '<table border="1">'

	html += '<tr><td></td>'
	var cols = ['Money', 'Science', 'Materials']
	for (var x = 0; x < 3; ++x)
		if 	(this.max_cubes[x] /*&& (this.max_fixed[x] || this.max_grey || (this.max_orbital && x < 2))*/)
			html += '<td>' + cols[x] + '</td>'
	html += '</tr>'
	
	var cell = '<input type="text" value="0" size="1" disabled>\
				<button type="button" onclick="data.action.OnButton(this, {0}, {1}, -1)">-</button>\
				<button type="button" onclick="data.action.OnButton(this, {0}, {1}, 1)">+</button>'
	
	var self = this
	
	var AddRow = function(name, y, fn)
	{
		var ok = false
		for (var x = 0; x < 3; ++x)
			ok = ok || fn(x)
		
		if (ok)
		{		
			html += '<tr> <td>' + name + '</td>'
			for (var x = 0; x < 3; ++x)
				if (self.max_cubes[x])
				{
					html += '<td>'
					if (fn(x))
						html += cell.format(x, y)
					html += '</td>'
				}
			html += '</tr>'
		}
	}

	AddRow('Normal', 0, function(x) { return self.max_fixed[x] })
	
	AddRow('Grey', 1, function(x) { return self.max_grey })

	AddRow('Orbital', 2, function(x) { return self.max_orbital && self.max_cubes[x] && x < 2 })

	html += '</table>'

	div.innerHTML = html
	
	this.array = []
	for (var x = 0; x < 3; ++x)
		this.array[x] = [0, 0, 0]
}

Colonise.ChooseSquaresStage.prototype.OnButton = function(button, x, y, delta)
{
	this.array[x][y] += delta
	if (this.Validate())
		button.parentNode.firstChild.value = this.array[x][y]
	else
		this.array[x][y] -= delta
}

Colonise.ChooseSquaresStage.prototype.Validate = function()
{
	var total_grey = 0, total_orbital = 0
	var total_res = [0, 0, 0]
	var total = 0
	for (var x = 0; x < 3; ++x)
	{
		if (this.array[x][0] > this.max_fixed[x]) // fixed square count exceeded
			return false
		
		total_grey += this.array[x][1]
		total_orbital += this.array[x][2]
		
		for (var y = 0; y < 3; ++y)
		{
			var n = this.array[x][y]
			
			if (n < 0)
				return false
			
			total_res[x] += n
			total += n
		}
	}

	if (total > this.num_ships)
		return false
	if (total_grey > this.max_grey)
		return false
	if (total_orbital > this.max_orbital)
		return false
	
	for (var x = 0; x < 3; ++x)
		if (total_res[x] > this.max_cubes[x])
			return false

	document.getElementById('choose_colonise_squares_btn').disabled = total == 0

	return true
}

Colonise.ChooseSquaresStage.prototype.Send = function()
{
	var json = CreateCommandJSON('cmd_colonise_squares')

	var rows = ['Fixed', 'Grey', 'Orbital']
	var cols = ['Money', 'Science', 'Materials']
	for (var y = 0; y < 3; ++y)
	{
		var row = {}
		for (var x = 0; x < 3; ++x)
			row[cols[x]] = this.array[x][y]
		json[rows[y]] = row
	}
	
	ExitAction()

	SendJSON(json)
}

Colonise.ChooseSquaresStage.prototype.CleanUp = function()
{
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

	data.action = new Colonise.ChooseSquaresStage(elem.ships, elem.square_counts, elem.max_cubes)
}
