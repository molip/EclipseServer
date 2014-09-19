var Canvas = {}

Canvas.AddLayer = function(base_canvas)
{
	var layer = document.createElement('canvas');
	layer.setAttribute('width', base_canvas.offsetWidth);
	layer.setAttribute('height', base_canvas.offsetHeight);
	layer.style.backgroundColor = "transparent";
	layer.style.position = "absolute";
	layer.style.left = base_canvas.offsetLeft+'px';
	layer.style.top = base_canvas.offsetTop+'px';
	layer.style.width = base_canvas.offsetWidth+'px';
	layer.style.height = base_canvas.offsetHeight+'px';

	base_canvas.parentNode.appendChild(layer);
	return layer
}

Canvas.RelMouseCoords = function(event, el)
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
