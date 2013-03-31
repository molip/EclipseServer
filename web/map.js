function GetHexCentre(x, y) 
{
	var cx = x * data.hex_width * 3 / 4
	var cy = (x + y * 2) * data.hex_height / 2
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

	//alert(px)

	
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

function OnMouse(evt)
{ 
	var canvas2 = document.getElementById('map_canvas2')
	
	var p = relMouseCoords(evt, canvas2)
	
	p.x = (p.x - 300) / 0.3
	p.y = (p.y - 300) / 0.3
	
	hex = HitTestHex(p.x, p.y)
	p2 = GetHexCentre(hex.x, hex.y)
	
	var ctx2 = canvas2.getContext("2d");
	ctx2.setTransform(1, 0, 0, 1, 0, 0)
	ctx2.clearRect(0, 0, 600, 600);

	ctx2.translate(300, 300)
	ctx2.scale(0.3, 0.3)
	
	var size_x = data.hex_width, size_y = data.hex_height
	ctx2.translate(-size_x / 2, -size_y / 2)
	ctx2.drawImage(data.img_select, p2.x, p2.y);
	ctx2.translate(size_x / 2, size_y / 2)
	
	
	// ctx2.beginPath()
	// ctx2.arc(p2.x, p2.y, data.hex_width / 2, 0,2 * Math.PI)
	// ctx2.closePath()
	// ctx2.strokeStyle="#FF0000";
	// ctx2.lineWidth=10
	// ctx2.globalAlpha = 0.5
	// ctx2.stroke();	
}
