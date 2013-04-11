var Map = {}
Map.img = new Image()
Map.hot = {}
Map.selecting = false

Map.hex_width = 445
Map.hex_height = 387
Map.zoom = 5
Map.scale = 1
Map.pan_x = 0
Map.pan_y = 0

Map.img_select = new Image()
Map.img_select.src = "/images/select.png"

Map.img_explore = new Image()
Map.img_explore.src = "/images/explore.png"

Map._hexes = []

Map.Hex = function(id, x, y, rotation) 
{
	this.id = id
	this.x = x
	this.y = y
	this.rotation = rotation
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

Map.GetHexCentre = function(x, y) 
{
	var cx = Number(x) * Map.hex_width * 3 / 4
	var cy = (Number(x) + Number(y) * 2) * Map.hex_height / 2
	return { x: cx, y: cy }
} 

Map.HitTestHex = function(px, py) // logical pixels
{
	var colWidth = Map.hex_width / 4;
	var rowHeight = Map.hex_height / 2;

	var iRow = Math.floor(py / rowHeight);
	var iCol = Math.floor(px / colWidth);
	
	var colStart = iCol * colWidth;
	var rowStart = iRow * rowHeight;

	var dx = (px - colStart) / colWidth;
	var dy = (py - rowStart) / rowHeight;

	var bEvenRow = iRow % 2 == 0;
	var bEvenCol = iCol % 2 == 0;
	var bShift = (bEvenRow != bEvenCol) ? dx > 1 - dy : dx > dy;

	var iTri = iCol + bShift;
	var iHexCol = Math.floor((iTri + 1) / 3);
	
	var hitX = iHexCol;
	var hitY = (Math.floor((iRow + 1 + iHexCol) / 2)) - iHexCol;
	
	return { x: hitX, y: hitY };
}

Map.RelMouseCoords = function(event, el)
{
    var totalOffsetX = 0;
    var totalOffsetY = 0;
    var canvasX = 0;
    var canvasY = 0;
    var currentElement = el;

    do{
        totalOffsetX += currentElement.offsetLeft;
        totalOffsetY += currentElement.offsetTop;
    }
    while(currentElement = currentElement.offsetParent)

    canvasX = event.pageX - totalOffsetX;
    canvasY = event.pageY - totalOffsetY;

    return {x:canvasX, y:canvasY}
}

Map.OnMouseMove = function(evt)
{ 
	if (!Map.selecting)
		return
		
	var p = Map.RelMouseCoords(evt, Map.canvas)
	
	p.x = (p.x - 300) / Map.scale - Map.pan_x
	p.y = (p.y - 300) / Map.scale - Map.pan_y
	
	hex = Map.HitTestHex(p.x, p.y)
	
	if (Map.hot == hex)
		return
	
	Map.hot = hex
	Map.DrawHotLayer()
}

Map.OnMouseOut = function()
{
	Map.hot.x = null
	Map.hot.y = null

	if (Map.layer_hot)
		Map.ClearCanvas(Map.layer_hot)	
}

Map.OnMouseDown = function()
{
	if (!Map.selecting)
		return

	if (data.action && data.action.OnHexMouseDown)
		if (data.action.OnHexMouseDown(Map.hot.x, Map.hot.y))
			Map.DrawSelectLayer()
}

Map.DrawCentred = function(ctx, img, x,  y, rotation)
{	
	var p = Map.GetHexCentre(x, y)

	ctx.save()
	ctx.translate(p.x, p.y)
	if (rotation != null)
		ctx.rotate(rotation * Math.PI / 3)
	ctx.drawImage(img, -img.width / 2, -img.height / 2);
	ctx.restore()
}

Map.DrawHex = function(ctx, hex)
{
	var size_x = Map.hex_width, size_y = Map.hex_height
	
	Map.img.src = "/images/hexes/" + hex.id + ".png"
	Map.DrawCentred(ctx, Map.img, hex.x, hex.y, hex.rotation);
}

Map.Clear = function()
{
	Map._hexes = []
}

Map.AddHex = function(hex)
{
	Map._hexes.push(hex)
}

Map.Draw = function()
{
	Map.DrawHexLayer()
	Map.DrawActionLayer()
	Map.DrawSelectLayer()
	Map.DrawHotLayer()
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

	var p = Map.GetHexCentre(data.action.selected.x, data.action.selected.y)
	
	ctx2.beginPath()
	ctx2.arc(p.x, p.y, Map.hex_width / 2, 0,2 * Math.PI)
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
	if (Map.hot && Map.hot.x != null && Map.hot.y != null)
		Map.DrawCentred(ctx, Map.img_select, Map.hot.x, Map.hot.y);
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