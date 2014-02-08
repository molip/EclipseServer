var output
var ws
var parser

var data = {}
data.team_pages = {}
data.team_count = 0

data.current_action_elem = null
data.action = null

Colour = { 'Red': '#cd171a', 'Blue': '#283583', 'Green': '#3fa535', 'Yellow': '#ffd500', 'White': '#fff', 'Black': '#111' }

data.disc_imgs = {}
for (var col in Colour)
{
	data.disc_imgs[col] = new Image() 
	data.disc_imgs[col].src = "/images/discs/" + col + ".png"
}

data.ancient_img = new Image() 
data.ancient_img.src = "/images/ships/ancient ship.png"

data.gcds_img = new Image() 
data.gcds_img.src = "/images/ships/gcds_ship.png"

function ExitAction()
{
	if (data.action && data.action.CleanUp)
		data.action.CleanUp()
	data.action = null
}

function LoadImage(path, onload)
{
	var img = new Image()
	img.src = path
	if (!img.complete)
		img.onload = onload
	return img
}

function Point(x, y)
{
	this.x = x
	this.y = y
}

Point.prototype.Clone = function()
{
	return new Point(this.x, this.y)
}

Point.prototype.equals = function(rhs)
{
	return rhs && this.x == rhs.x && this.y == rhs.y
}

function ReadPoints(elem, elem_name)
{
	var points = []
	var elems = GetChildElements(elem, elem_name)
	for (var i = 0; i < elems.length; ++i)
	{
		var x = elems[i].getAttribute('x')
		var y = elems[i].getAttribute('y')
		points.push(new Point(x, y))
	}
	return points
}

function CompareObjects(lhs, rhs)
{
	return lhs == null ? rhs == null : lhs.equals(rhs)
}

if(!String.prototype.format) {
  String.prototype.format = function() {
    var args = arguments;
    return this.replace(/{(\d+)}/g, function (match, number) {
          return typeof args[number]!= 'undefined'
        ? args[number]
        : match
		;
});
};
}

function Assert(test, msg)
{
	if (!test)
		alert(msg)
}

function writeToScreen(message)
{
	document.getElementById("output").innerText += message
	//document.getElementById("output").innerHTML += '<br/>'
}

function IsTrue(str)
{
	return str == '1' || str == 'true';
}

function GetChildElements(elem, name)
{
	var array = []
	for (var e = elem.firstChild; e; e = e.nextSibling)
		if (e.nodeName == name)
			array.push(e)
	return array
}

function GetFirstChildElement(elem, name)
{
	for (var e = elem.firstChild; e; e = e.nextSibling)
		if (e.nodeName == name)
			return e
	return null
}

function ClearSelect(select_elem)
{
	while (select_elem.length)
		select_elem.remove(0)
}

function OnMessage(msg)
{
	writeToScreen(msg.data)
	
	var xml = parser.parseFromString(msg.data, "text/xml");

	var name = xml.firstChild.nodeName
	if (name == "command")
		OnCommand(xml.firstChild)
	else if (name == "response")
		ShowBlanket(false)
}
  
function OnClose()
{
	document.getElementById('shroud').style.display = 'table' // For v centre
}

function load()
{
	if ("WebSocket" in window)
	{
		ws = new WebSocket(webSocketURL);
		ws.onopen = function() { SendRegister(); }
		ws.onmessage = OnMessage;
		ws.onclose = OnClose;
	}
	else
	{
		alert("WebSocket not supported by your Browser");
	}

	if (window.DOMParser)
		parser=new DOMParser();
	else
		alert("DOMParser not supported");
}

function GetTeamDivIDFromName(team, div)
{
	var index = data.team_pages[team]
	var id =  GetTeamPageIDFromIndex(index)
	if (div)	
		id += '_' + div
	return id
}

function GetTeamPageIDFromIndex(index)
{
	return 'teampage_{0}'.format(index)
}

function ShowElementById(id, show, inline)
{
	var e = document.getElementById(id)
	ShowElement(e, show, inline)
	return e;
}

function ShowElement(e, show, inline)
{
	if (e != null)
		e.style.display = show ? inline ? "inline" : "block" : "none"
}

function ShowActionElement(id)
{
	ShowElement(data.current_action_elem, false)
	data.current_action_elem = id == null ? null : ShowElementById(id, true)
}

function ShowTeamPage(player_id)
{
	var page = data.team_pages[player_id]
    for (var i = 0; i < data.team_count; ++i)
		ShowElementById(GetTeamPageIDFromIndex(i), page == i)

	ShowElementById('game_pages', true)
	ShowElementById('supply_page', false)
}

function ShowSupplyPage()
{
	ShowElementById('game_pages', false)
	ShowElementById('supply_page', true)
}

function ShowBlanket(show)
{
	ShowElementById('blanket', show)
}
