var Capturer = function()
{
	this._capture = null
	this._objects = {}
	
	window.onmousedown = this.OnMouseDown.bind(this)
	window.onmouseup = this.OnMouseUp.bind(this)
	window.onmousemove = this.OnMouseMove.bind(this)
}

Capturer.prototype.Register = function(id, object)
{
	this._objects[id] = object
}

Capturer.prototype.HitTest = function(e) 
{
	for (var id in this._objects)
	{
		var elem = document.elementFromPoint(e.clientX, e.clientY)
		if (elem && elem.id == id)
			return this._objects[id]
	}
	return null
}

Capturer.prototype.OnMouseDown = function(e) 
{
	var object = this.HitTest(e)
	if (object)
	{
		this._capture = object
		object.OnMouseDown(e)
		return false
	}
}

Capturer.prototype.OnMouseMove = function(e) 
{
	if (this._capture)
		this._capture.OnMouseMove(e)
	else 
	{
		var object = this.HitTest(e)
		if (object)
			object.OnMouseMove(e)
	}
}

Capturer.prototype.OnMouseUp = function(e) 
{
	if (this._capture)
		this._capture.OnMouseUp(e)
	else 
	{
		var object = this.HitTest(e)
		if (object)
			object.OnMouseUp(e)
	}
		
	this._capture = null
}
