var Population = {}

Population.canvas = null
Population.uiCanvas = null

Population.DrawCube = function(x, y, team, ctx, num)
{
	ctx.fillStyle = GetFillColour(team)
	ctx.beginPath()
	ctx.rect(x + 6, y + 6, 28, 28)
	ctx.closePath()
	Map.DrawTeamPath(ctx, team)

	if (num != null)
	{
		ctx.fillStyle = GetStrokeColour(team)
		ctx.fillText(num.toString(), x + 20, y + 20)
	}
}

Population.Init = function()
{
	if (Population.canvas == null)
	{
		Population.canvas = document.getElementById('population_canvas')
		Population.uiCanvas = document.getElementById('population_ui_canvas')
	}
}

Population.Draw = function(cubeCounts, team)
{
	var ctx = Population.canvas.getContext("2d");

	ctx.setTransform(1, 0, 0, 1, 0, 0)
	ctx.clearRect(0, 0, Population.canvas.width, Population.canvas.height);

	ctx.font = '16px sans-serif'
	ctx.textAlign = 'center'
	ctx.textBaseline = 'middle'
	
	var labels = [28, 24, 21, 18, 15, 12, 10, 8, 6, 4, 3, 2];
	
	var x = 5
	for (var type in cubeCounts)
	{
		for (var i = 0; i < cubeCounts[type]; ++i)
			Population.DrawCube(x + i * 48, 4, team, ctx, labels[i])
		x += 580
	}
}

