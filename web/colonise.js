var Colonise = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Colonise.ChoosePosStage = function(positions)
{
	this.positions = positions
	this.selected = null
	this.pos_idx = -1

	document.getElementById('choose_colonise_pos_btn').disabled = true

	ShowElementById('choose_undo', true)
	document.getElementById('choose_undo_btn').disabled = false
}

Colonise.ChoosePosStage.prototype.OnHexMouseDown = function(pt)
{
	for (var i = 0; i < this.positions.length; ++i)
		if (this.positions[i].equals(pt))
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
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, 'cmd_colonise_pos')
	node.setAttribute("pos_idx", this.pos_idx)

	ExitAction()

	SendXMLDoc(doc)
}

Colonise.ChoosePosStage.prototype.CleanUp = function()
{
	Map.ClearCanvas(Map.layer_select)
	Map.ClearCanvas(Map.layer_action)
}

/////////////

Colonise.ChooseSquaresStage = function(num_ships, square_counts, max_cubes)
{
	this.num_ships = num_ships

	this.max_fixed = [square_counts.money, square_counts.science, square_counts.materials]
	this.max_cubes = [max_cubes.money, max_cubes.science, max_cubes.materials]
	this.max_grey = square_counts.any
	this.max_orbital = square_counts.orbital

	document.getElementById('choose_colonise_squares_btn').disabled = true

	ShowElementById('choose_undo', true)
	document.getElementById('choose_undo_btn').disabled = false

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
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, 'cmd_colonise_squares')

	var rows = ['fixed', 'grey', 'orbital']
	var cols = ['money', 'science', 'materials']
	for (var y = 0; y < 3; ++y)
	{
		var row = doc.createElement(rows[y])
		for (var x = 0; x < 3; ++x)
			row.setAttribute(cols[x], this.array[x][y])
		node.appendChild(row)
	}
	
	ExitAction()

	SendXMLDoc(doc)
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

	var positions = ReadPoints(elem, 'pos')
	data.action = new Colonise.ChoosePosStage(positions)
	Map.DrawActionLayer()
}

Colonise.OnCommandChooseSquares = function(elem)
{
	ShowActionElement('choose_colonise_squares')
	Map.selecting = false

	var num_ships = Number(elem.getAttribute('ships'))

	var square_counts = {}
	var square_counts_elem = GetFirstChildElement(elem, 'square_counts')
	var elems = GetChildElements(square_counts_elem, 'type')
	for (var i = 0; i < elems.length; ++i)
	{
		var type = elems[i].getAttribute('name')
		var count = elems[i].getAttribute('count')
		square_counts[type] = Number(count)
	}
	
	var max_cubes = {}
	var max_cubes_elem = GetFirstChildElement(elem, 'max_cubes')
	var elems = GetChildElements(max_cubes_elem, 'type')
	for (var i = 0; i < elems.length; ++i)
	{
		var type = elems[i].getAttribute('name')
		var count = elems[i].getAttribute('count')
		max_cubes[type] = Number(count)
	}

	data.action = new Colonise.ChooseSquaresStage(num_ships, square_counts, max_cubes)
}
