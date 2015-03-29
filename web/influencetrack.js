var InfluenceTrack = {}

InfluenceTrack.canvas = null
InfluenceTrack.uiCanvas = null

InfluenceTrack.Transform = function(ctx, index)
{
	ctx.scale(0.7, 0.7)
	ctx.translate(18, 1034 - index * 84.4)
}

InfluenceTrack.DrawSelection = function(ctx, index, colour)
{
	ctx.save()
	InfluenceTrack.Transform(ctx, index)

	ctx.beginPath()
	ctx.rect(-10, -10, 80, 80)
	ctx.closePath()
	ctx.strokeStyle = colour
	ctx.lineWidth = 8
	ctx.stroke()
	ctx.restore()
}

InfluenceTrack.DrawDisc = function(index, team, ctx, num)
{
	ctx.save()
	InfluenceTrack.Transform(ctx, index)
	
	ctx.save()
	ctx.scale(0.8, 0.8)
	var img = data.disc_imgs[team]
	ctx.drawImage(img, 0, 0);
	ctx.restore()

	if (num != null)
	{
		ctx.fillStyle = GetStrokeColour(team)
		ctx.fillText(num.toString(), 35, 30)
	}
	
	ctx.restore()
}

InfluenceTrack.Init = function()
{
	if (InfluenceTrack.canvas == null)
	{
		InfluenceTrack.canvas = document.getElementById('influence_canvas')
	}
}

InfluenceTrack.Draw = function(discs, team)
{
	var ctx = InfluenceTrack.canvas.getContext("2d");

	ctx.setTransform(1, 0, 0, 1, 0, 0)
	ctx.clearRect(0, 0, InfluenceTrack.canvas.width, InfluenceTrack.canvas.height);

	ctx.font = '30px sans-serif'
	ctx.textAlign = 'center'
	ctx.textBaseline = 'middle'
	
	var labels = [-30, -25, -21, -17, -13, -10, -7, -5, -3, -2, -1, 0, 0];
	for (var i = 0; i < discs; ++i)
		InfluenceTrack.DrawDisc(i, team, ctx, labels[i])

	if (IsCurrentTeam(data.playerID) &&data.action && data.action.can_select_track)
		InfluenceTrack.DrawSelection(ctx, discs - (data.action.is_dst ? 0 : 1), selectColour = data.action.track_selected ? "red" : "green")
}
