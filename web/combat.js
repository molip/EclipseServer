var Combat = {}

Combat.canvas = null
Combat.overlay = null
Combat.invader = null
Combat.defender = null

Combat.Init = function()
{
	if (Combat.canvas == null)
	{
		Combat.canvas = document.getElementById('combat_canvas')

		Combat.overlay = Canvas.AddLayer(Map.canvas)

		Combat.overlay.onmousemove = Map.OnMouseMove
		Combat.overlay.onmouseout = Map.OnMouseOut
		Combat.overlay.onmousedown = Map.OnMouseDown

		Combat.OnMouseOut()
	}
}

Combat.Update = function(invader, defender)
{ 
	Combat.invader = invader
	Combat.defender = defender
	
	Combat.Draw()
}

Combat.Draw = function()
{
	var ctx = Combat.canvas.getContext("2d");

	ctx.setTransform(1, 0, 0, 1, 0, 0)
	ctx.clearRect(0, 0, Combat.canvas.width, Combat.canvas.height);

	var drawSide = function(side)
	{
		var gap = 5
		var totalHeight = gap, maxWidth = 0
		for (var i = 0, group; group = side.ship_groups[i]; ++i)
		{
			if (!group.image)
				group.image = LoadImage('/images/ships/player/' + side.colour + group.type + '.png', Combat.Draw)
			totalHeight += (group.image.height + gap) * group.ships.length
			
			if (maxWidth < group.image.width)
				maxWidth = group.image.width
		}
		
		var wide = totalHeight > Combat.canvas.height
		
		var y = gap
		for (var i = 0, group; group = side.ship_groups[i]; ++i)
		{
			var x = 0
			var inset = group.active ? 50 : gap
			for (var j = 0; j < group.ships.length; ++j)
			{
				ctx.drawImage(group.image, inset + x, y);
				if (wide && (j % 2 == 0) && j < group.ships.length - 1)
					x += group.image.width + gap
				else
				{
					x = 0
					y += group.image.height + gap
				}
			}
		}
	}
	
	drawSide(Combat.invader)

	ctx.save()
	ctx.translate(Combat.canvas.width, 0)
	ctx.scale(-1, 1)
	drawSide(Combat.defender)
	ctx.restore()
	
	var drawDie = function(ctx, colour, pips)
	{
		ctx.fillStyle = colour

		var w = 40, h = 40, r = 10
		
		ctx.beginPath();
		ctx.moveTo(r, 0);
		ctx.lineTo(w-r, 0);
		ctx.quadraticCurveTo(w, 0, w, r);
		ctx.lineTo(w, h-r);
		ctx.quadraticCurveTo(w, h, w-r, h);
		ctx.lineTo(r, h);
		ctx.quadraticCurveTo(0, h, 0, h-r);
		ctx.lineTo(0, r);
		ctx.quadraticCurveTo(0, 0, r, 0);
		ctx.fill();        
		
		var drawPip = function(ctx, x, y)
		{
			ctx.beginPath();
			ctx.arc(10 + x * 10, 10 + y * 10, 4, 0, 2 * Math.PI)
			ctx.closePath()
			ctx.fill();        
		}

		ctx.fillStyle = 'black'
		
		if (pips % 2)
			drawPip(ctx, 1, 1)
		if (pips == 6)
		{
			drawPip(ctx, 0, 1)
			drawPip(ctx, 2, 1)
		}
		if (pips > 3)
		{
			drawPip(ctx, 0, 0)
			drawPip(ctx, 2, 2)
		}
		if (pips > 1)
		{
			drawPip(ctx, 2, 0)
			drawPip(ctx, 0, 2)
		}
	}

	ctx.translate(280, 50)

	ctx.globalAlpha = 0.3

	for (var i = 1; i < 7; ++i)
	{
		if (i == 4)
			ctx.globalAlpha = 1

		drawDie(ctx, 'red', i)
		ctx.translate(0, 50)
	}
}

Combat.OnMouseMove = function(evt)
{ 
	var pt = Canvas.RelMouseCoords(evt, Map.canvas)
}

Combat.OnMouseOut = function(evt)
{ 
}

Combat.OnMouseDown = function(evt)
{ 
}

//-----------------------------------------------------------------------------

Combat.Stage = function(missiles, can_fire, can_retreat) 
{
	this.flagNoSubAction = true;
	this.flagCantUndo = true

	ShowElementById('choose_combat_missiles', missiles, true)
	ShowElementById('choose_combat_cannons', !missiles, true)
	ShowElementById('choose_combat_retreat', !missiles, true)

	document.getElementById('choose_combat_cannons').disabled = !can_fire
	document.getElementById('choose_combat_retreat').disabled = !can_retreat
}

Combat.Stage.prototype.Send = function(fire)
{
	var json = CreateCommandJSON("cmd_combat")
	json.fire = fire

	ExitAction()

	SendJSON(json)
}

Combat.OnCommand = function(elem)
{
	ShowActionElement('choose_combat')

	data.action = new Combat.Stage(elem.missiles, elem.can_fire, elem.can_retreat)

	// Map.DrawActionLayer()
	// Map.DrawSelectLayer()
}
