var Map = {}
Map.img = new Image()
Map.selected = {}
Map.hot = {}
Map.selecting = false

OnMouseOut()

function InitMap()
{
	if (Map.canvas == null)
	{
		Map.canvas = document.getElementById('map_canvas')

		Map.layer_action = AddLayer()
		Map.layer_select = AddLayer()
		Map.layer_hot = AddLayer()

		Map.layer_hot.onmousemove = OnMouseMove
		Map.layer_hot.onmouseout = OnMouseOut
		Map.layer_hot.onmousedown = OnMouseDown
	}

	Map.selecting = false
	ClearCanvas(Map.layer_action)
	ClearCanvas(Map.layer_select)
	ClearCanvas(Map.layer_hot)
}

function AddLayer()
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

function ClearCanvas(canvas)
{
	ClearContext(canvas.getContext("2d"))	
}

function ClearContext(ctx)
{
	ctx.setTransform(1, 0, 0, 1, 0, 0)
	ctx.clearRect(0, 0, 600, 600);

	ctx.translate(300, 300)
	ctx.scale(data.map_scale, data.map_scale)
}

function GetHexCentre(x, y) 
{
	var cx = Number(x) * data.hex_width * 3 / 4
	var cy = (Number(x) + Number(y) * 2) * data.hex_height / 2
	return { x: cx, y: cy }
} 

function HitTestHex(px, py) // logical pixels
{
	var colWidth = data.hex_width / 4;
	var rowHeight = data.hex_height / 2;

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

function relMouseCoords(event, el){
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

function OnMouseMove(evt)
{ 
	if (!Map.selecting)
		return
		
	var p = relMouseCoords(evt, Map.canvas)
	
	p.x = (p.x - 300) / data.map_scale
	p.y = (p.y - 300) / data.map_scale
	
	hex = HitTestHex(p.x, p.y)
	
	if (Map.hot == hex)
		return
	
	Map.hot = hex
	
	var ctx = Map.layer_hot.getContext("2d");
	ClearContext(ctx)
	DrawCentred(ctx, data.img_select, hex.x, hex.y);
}

function OnMouseOut()
{
	Map.hot.x = null
	Map.hot.y = null

	if (Map.layer_hot)
		ClearCanvas(Map.layer_hot)	
}

function OnMouseDown()
{
	if (!Map.selecting)
		return

	if (data.action && data.action.positions)
	{
		var ok = false
		for (var i = 0; i < data.action.positions.length && !ok; ++i)
			if (data.action.positions[i].x == Map.hot.x && data.action.positions[i].y == Map.hot.y)
				ok = true
		if (!ok)
			return
	}

	Map.selected = Map.hot
	DrawSelected(Map.selected.x, Map.selected.y)

	document.getElementById('choose_explore_pos_btn').disabled = false
}

function DrawSelected(x, y)
{	
	var ctx2 = Map.layer_select.getContext("2d");
	ClearContext(ctx2)

	if (x == null || y == null)
		return

	var p = GetHexCentre(x, y)
	
	ctx2.beginPath()
	ctx2.arc(p.x, p.y, data.hex_width / 2, 0,2 * Math.PI)
	ctx2.closePath()
	ctx2.strokeStyle="#FF0000";
	ctx2.lineWidth=10
	ctx2.globalAlpha = 0.5
	ctx2.stroke();	
}

function DrawPositions()
{
	if (!data.action || !data.action.positions)
	{
		Assert(false, "DrawPositions")
		return 
	}
	
	var ctx = Map.layer_action.getContext("2d");
	ClearContext(ctx)

	for (var i = 0; i < data.action.positions.length; ++i)
		DrawCentred(ctx, data.img_explore, data.action.positions[i].x, data.action.positions[i].y)
}

function DrawCentred(ctx, img, x,  y, rotation)
{	
	var p = GetHexCentre(x, y)

	ctx.save()
	ctx.translate(p.x, p.y)
	if (rotation != null)
		ctx.rotate(rotation * Math.PI / 3)
	ctx.drawImage(img, -img.width / 2, -img.height / 2);
	ctx.restore()
}

function DrawHex(ctx, id, x, y, rotation)
{
	var size_x = data.hex_width, size_y = data.hex_height
	
	Map.img.src = "/images/hexes/" + id + ".png"
	DrawCentred(ctx, Map.img, x, y, rotation);
}
