var Population = {}

Population.canvas = null

Population.Init = function()
{
	if (Population.canvas == null)
	{
		Population.canvas = document.getElementById('population_canvas')
	}
}

Population.Draw = function(counts, colour)
{
	var ctx = Population.canvas.getContext("2d");

	ctx.setTransform(1, 0, 0, 1, 0, 0)
	ctx.clearRect(0, 0, Population.canvas.width, Population.canvas.height);

	var drawCube = function(x, y)
	{
		ctx.fillStyle = colour
		ctx.beginPath()
		ctx.rect(x + 6, y + 6, 28, 28)
		ctx.closePath()
		Map.DrawTeamPath(ctx, colour)
	}

	var y = 4
	for (var type in counts)
	{
		for (var i = 0; i < counts[type]; ++i)
			drawCube(5 + i * 48, y)
		y += 52
	}
}