var Combat = {}

Combat.canvas = null
Combat.diceLayer = null
Combat.overlay = null
Combat.invader = null
Combat.defender = null
Combat.dice = null

Combat.Init = function()
{
	if (Combat.canvas == null)
	{
		Combat.canvas = document.getElementById('combat_canvas')

		Combat.diceLayer = Canvas.AddLayer(Combat.canvas)
		Combat.overlay = Canvas.AddLayer(Combat.canvas)

		Combat.overlay.onmousemove = Combat.OnMouseMove
		Combat.overlay.onmouseout = Combat.OnMouseOut
		Combat.overlay.onmousedown = Combat.OnMouseDown

		Combat.OnMouseOut()
	}
}

Combat.Update = function(invader, defender)
{ 
	Combat.invader = invader
	Combat.defender = defender
	Combat.dice = null
	
	Combat.Draw()
	Combat.DrawDice() // Clear
}

Combat.UpdateDice = function(dice)
{ 
	Combat.dice = dice.length ? dice : null
	Combat.DrawDice()
}

Combat.Draw = function()
{
	var ctx = Combat.canvas.getContext("2d");

	ctx.setTransform(1, 0, 0, 1, 0, 0)
	ctx.clearRect(0, 0, Combat.canvas.width, Combat.canvas.height);

	var drawSide = function(side)
	{
		var gap = 5
		var livesWidth = 8
		var totalHeight = gap, maxWidth = 0
		for (var i = 0, group; group = side.ship_groups[i]; ++i)
		{
			if (!group.image)
			{
				if (group.type == 'GCDS')
					group.image = data.gcds_img
				else if (group.type == 'Ancient')
					group.image = data.ancient_img
				else
					group.image = LoadImage('/images/ships/player/' + side.colour + group.type + '.png', Combat.Draw)
			}
			totalHeight += (group.image.height + gap) * group.ships.length
			
			if (maxWidth < group.image.width)
				maxWidth = group.image.width
		}
		
		var wide = totalHeight > Combat.canvas.height
		
		var y = gap
		for (var i = 0, group; group = side.ship_groups[i]; ++i)
		{
			var inset = livesWidth + (group.active ? 50 : gap)
			var x = inset
			for (var j = 0; j < group.ships.length; ++j)
			{			
				if (side == Combat.defender && side.colour == 'None') // Unflip.
				{
					ctx.save()
					ctx.scale(-1, 1)
					ctx.drawImage(group.image, -x - group.image.width, y);
					ctx.restore()
				}
				else 
					ctx.drawImage(group.image, x, y);
				
				if (group.ships[j] <= 0) // Dead.
				{
					ctx.strokeStyle = '#f00'
					ctx.lineWidth = 5
					ctx.beginPath();
					ctx.moveTo(x, y);
					ctx.lineTo(x + group.image.width, y + group.image.height);
					ctx.stroke()
					ctx.moveTo(x + group.image.width, y);
					ctx.lineTo(x, y + group.image.height);
					ctx.stroke()
				}
				else
				{
					ctx.fillStyle = '#0f0'
					var lives = group.ships[j]
					var lifeSize = 6
					var lifeGap = 2
					var stride = Math.min(lifeSize + lifeGap, group.image.height / lives)
					var top = y + (group.image.height - lives * stride - lifeGap) / 2
					
					ctx.beginPath();
					for (var k = 0; k < lives; ++k)
					{
						ctx.rect(x - livesWidth, top, lifeSize, stride - lifeGap);
						top += stride;
					}
					ctx.fill()
				}
				
				if (wide && (j % 2 == 0) && j < group.ships.length - 1)
					x += group.image.width + gap + livesWidth
				else
				{
					x = inset
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
}

Combat.DrawDice = function()
{
	var ctx = Combat.diceLayer.getContext("2d");

	ctx.setTransform(1, 0, 0, 1, 0, 0)
	ctx.clearRect(0, 0, Combat.canvas.width, Combat.canvas.height);

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

	//ctx.globalAlpha = 0.3

	if (!Combat.dice)
		return
	
	var count = 0
	for (var i = 0, type; type = Combat.dice[i]; ++i)
		count += type.values.length

	var wide = count > 10
	
	ctx.translate(Map.canvas.width / 2 - (wide ? 45 : 20), 10)
	
	var right = false
	for (var i = 0, type; type = Combat.dice[i]; ++i)
		for (var j = 0, val; val = type.values[j]; ++j)
		{
			drawDie(ctx, type.colour, val)
			
			if (wide)
			{ 
				if (right = !right)
					ctx.translate(50, 0)
				else
					ctx.translate(-50, 50)
			}
			else
				ctx.translate(0, 50)
		}
}

Combat.OnMouseMove = function(evt)
{ 
	var pt = Canvas.RelMouseCoords(evt, Combat.canvas)
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

	ShowElementById('choose_combat_missiles', missiles)
	ShowElementById('choose_combat_cannons', !missiles)
	ShowElementById('choose_combat_retreat', !missiles)

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

//-----------------------------------------------------------------------------

Combat.DiceStage = function() 
{
	this.flagNoSubAction = true;
	this.flagCantUndo = true
}

Combat.DiceStage.prototype.Send = function(fire)
{
	var json = CreateCommandJSON("cmd_dice")

	ExitAction()

	SendJSON(json)
}

//-----------------------------------------------------------------------------

Combat.OnCommand = function(elem)
{
	ShowActionElement('choose_combat')

	data.action = new Combat.Stage(elem.missiles, elem.can_fire, elem.can_retreat)
}

Combat.OnDiceCommand = function(elem)
{
	if (elem.active_player_id == data.playerID)
		ShowActionElement('choose_dice')
	
	Combat.UpdateDice(elem.dice)

	data.action = new Combat.DiceStage()
}
