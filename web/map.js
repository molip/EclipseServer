var Map = {}
var Map = {}
Map.img = new Image()
Map.hot = null
Map.selecting = false

Map.hex_width = 445
Map.hex_height = 387
Map.disc_rad = 40
Map.zoom = 5
Map.scale = 1
Map.pan_x = 0
Map.pan_y = 0

Map.img_select = new Image()
Map.img_select.src = "/images/select.png"

Map.img_explore = new Image()
Map.img_explore.src = "/images/explore.png"

Map._hexes = []

Map.Hex = function(id, pos, rotation, team, squares, onload) 
{
	var self = this
	
	this.id = id
	this.pos = pos.Clone()
	this.rotation = rotation
	this.team = team
	this.squares = squares ? squares : []
	this.img = LoadImage("/images/hexes/" + id + ".png", function() { onload(self) })
}

Map.Init = function()
{
	if (Map.canvas == null)
	{
		Map.canvas = document.getElementById('map_canvas')

		Map.layer_action = Map.AddLayer()
		Map.layer_select = Map.AddLayer()
		Map.layer_hot = Map.AddLayer()

		Map.layer_hot.onmousemove = Map.OnMouseMove
		Map.layer_hot.onmouseout = Map.OnMouseOut
		Map.layer_hot.onmousedown = Map.OnMouseDown

		Map.OnMouseOut()
		Map.UpdateScale()
	}

	Map.selecting = false
	Map.ClearCanvas(Map.layer_action)
	Map.ClearCanvas(Map.layer_select)
	Map.ClearCanvas(Map.layer_hot)
}

Map.AddLayer = function()
{
	var layer = document.createElement('canvas');
	layer.setAttribute('width', Map.canvas.offsetWidth);
	layer.setAttribute('height', Map.canvas.offsetHeight);
	layer.style.backgroundColor = "transparent";
	layer.style.position = "absolute";
	layer.style.left = Map.canvas.offsetLeft+'px';
	layer.style.top = Map.canvas.offsetTop+'px';
	layer.style.width = Map.canvas.offsetWidth+'px';
	layer.style.height = Map.canvas.offsetHeight+'px';

	Map.canvas.parentNode.appendChild(layer);
	return layer
}

Map.ClearCanvas = function(canvas)
{
	Map.ClearContext(canvas.getContext("2d"))	
}

Map.ClearContext = function(ctx)
{
	ctx.setTransform(1, 0, 0, 1, 0, 0)
	ctx.clearRect(0, 0, 600, 600);

	ctx.translate(300, 300)
	ctx.scale(Map.scale, Map.scale)
	ctx.translate(-Map.pan_x, -Map.pan_y)
}

Map.GetHexCentre = function(pos) 
{
	var cx = Number(pos.x) * Map.hex_width * 3 / 4
	var cy = (Number(pos.x) + Number(pos.y) * 2) * Map.hex_height / 2
	return new Point(cx, cy)
} 

Map.HitTestHex = function(pt) // logical pixels
{
	var colWidth = Map.hex_width / 4;
	var rowHeight = Map.hex_height / 2;

	var iRow = Math.floor(pt.y / rowHeight);
	var iCol = Math.floor(pt.x / colWidth);
	
	var colStart = iCol * colWidth;
	var rowStart = iRow * rowHeight;

	var dx = (pt.x - colStart) / colWidth;
	var dy = (pt.y - rowStart) / rowHeight;

	var bEvenRow = iRow % 2 == 0;
	var bEvenCol = iCol % 2 == 0;
	var bShift = (bEvenRow != bEvenCol) ? dx > 1 - dy : dx > dy;

	var iTri = iCol + bShift;
	var iHexCol = Math.floor((iTri + 1) / 3);
	
	var hitX = iHexCol;
	var hitY = (Math.floor((iRow + 1 + iHexCol) / 2)) - iHexCol;
	
	return new Point(hitX, hitY)
}

Map.RelMouseCoords = function(event, el)
{
    var totalOffsetX = 0;
    var totalOffsetY = 0;
    var currentElement = el;

    do{
        totalOffsetX += currentElement.offsetLeft;
        totalOffsetY += currentElement.offsetTop;
    }
    while(currentElement = currentElement.offsetParent)

    return new Point(event.pageX - totalOffsetX, event.pageY - totalOffsetY)
}

Map.OnMouseMove = function(evt)
{ 
	if (!Map.selecting)
		return
		
	var pt = Map.RelMouseCoords(evt, Map.canvas)
	
	pt.x = (pt.x - 300) / Map.scale + Map.pan_x
	pt.y = (pt.y - 300) / Map.scale + Map.pan_y

	//alert(pt.y)
	
	hex = Map.HitTestHex(pt)
	
	if (CompareObjects(Map.hot, hex))
		return
	
	Map.hot = hex ? hex.Clone() : null
	Map.DrawHotLayer()
}

Map.OnMouseOut = function()
{
	Map.hot = null

	if (Map.layer_hot)
		Map.ClearCanvas(Map.layer_hot)	
}

Map.OnMouseDown = function()
{
	if (!Map.selecting)
		return

	if (data.action && data.action.OnHexMouseDown)
		if (data.action.OnHexMouseDown(Map.hot))
			Map.DrawSelectLayer()
}

Map.DrawCentred = function(ctx, img, pos, rotation, offset)
{	
	var pt = Map.GetHexCentre(pos)

	if (offset)
	{
		pt.x += offset.x;
		pt.y += offset.y;
	}
	ctx.save()
	ctx.translate(pt.x, pt.y)
	if (rotation != null)
		ctx.rotate(rotation * Math.PI / 3)
	ctx.drawImage(img, -img.width / 2, -img.height / 2);
	ctx.restore()
}

Map.DrawHex = function(ctx, hex)
{
	var size_x = Map.hex_width, size_y = Map.hex_height
	
	Map.DrawCentred(ctx, hex.img, hex.pos, hex.rotation);
	
	if (hex.team != null)
	{
		Map.DrawCentred(ctx, data.disc_imgs[hex.team], hex.pos, 0, new Point(7, -7))
	}
	
	var size = 26
	var pt = Map.GetHexCentre(hex.pos)
	ctx.save()
	ctx.translate(pt.x, pt.y)
	if (hex.rotation != null)
		ctx.rotate(hex.rotation * Math.PI / 3)
	ctx.translate(-Map.hex_width / 2, -Map.hex_height / 2)

	for (var i = 0; i < hex.squares.length; ++i)
	{
		var pt = hex.squares[i]
		ctx.beginPath()
		ctx.rect(pt.x - size / 2, pt.y - size / 2, size, size)
		ctx.closePath()
		Map.DrawTeamPath(ctx, hex.team)
	}
	ctx.restore()
}

Map.DrawTeamPath = function(ctx, team)
{
	ctx.fillStyle = Colour[team]
	ctx.fill();	

	ctx.strokeStyle = team == 'Black' || team == 'Blue' ? '#fff' : '#000'
	ctx.lineWidth = 3
	ctx.stroke()
}

Map.Clear = function()
{
	Map._hexes = []
}

Map.AddHex = function(id, pos, rotation, team, squares)
{
	Map._hexes.push(new Map.Hex(id, pos.Clone(), rotation, team, squares, Map.DrawHexLayerSingle))
}

Map.Draw = function()
{
	Map.DrawHexLayer()
	Map.DrawActionLayer()
	Map.DrawSelectLayer()
	Map.DrawHotLayer()
}

Map.DrawHexLayerSingle = function(hex)
{
	var ctx = Map.canvas.getContext("2d");
	Map.DrawHex(ctx, hex)
}

Map.DrawHexLayer = function()
{
	var ctx = Map.canvas.getContext("2d");
	Map.ClearContext(ctx)

	for (var i = 0; i < Map._hexes.length; ++i)
		Map.DrawHex(ctx, Map._hexes[i])
}

Map.DrawActionLayer = function()
{
	var ctx = Map.layer_action.getContext("2d");
	Map.ClearContext(ctx)

	if (data.action && data.action.OnDraw)
		data.action.OnDraw(ctx)
}

Map.DrawSelectLayer = function()
{	
	var ctx2 = Map.layer_select.getContext("2d");
	Map.ClearContext(ctx2)

	if (!data.action || !data.action.selected)
		return

	var pt = Map.GetHexCentre(data.action.selected)
	
	ctx2.beginPath()
	ctx2.arc(pt.x, pt.y, Map.hex_width / 2, 0,2 * Math.PI)
	ctx2.closePath()
	ctx2.strokeStyle="#FF0000";
	ctx2.lineWidth=10
	ctx2.globalAlpha = 0.5
	ctx2.stroke();	
}

Map.DrawHotLayer = function()
{
	var ctx = Map.layer_hot.getContext("2d");
	Map.ClearContext(ctx)
	if (Map.hot)
		Map.DrawCentred(ctx, Map.img_select, Map.hot);
}

Map.Zoom = function(n)
{
	Map.zoom += n
	if (Map.zoom < 0)
		Map.zoom = 0
	
	Map.UpdateScale()
	
	Map.Draw()
}

Map.PanX = function(n)
{
	Map.pan_x += n * Map.hex_width / 2
	Map.Draw()
}

Map.PanY = function(n)
{
	Map.pan_y += n * Map.hex_width / 2
	Map.Draw()
}

Map.PanCentre = function()
{
	Map.pan_x = 0
	Map.pan_y = 0
	Map.Draw()
}

Map.UpdateScale = function()
{
	Map.scale = 1
	for (var i = 0; i < Map.zoom; ++i)
		Map.scale *= 3 / 4
}