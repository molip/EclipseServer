var output
var ws
var parser


var data = {}
data.team_pages = {}
data.team_count = 0

data.hex_width = 445
data.hex_height = 387

data.img_select = new Image()
data.img_select.src = "/images/select.png"

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

function OnMessage(msg)
{
	writeToScreen(msg.data)
	
	var xml = parser.parseFromString(msg.data, "text/xml");

	if (xml.firstChild.nodeName == "command")
		OnCommand(xml.firstChild)
}
  
function OnClose()
{
	writeToScreen('Closed\n')
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

function GetTeamPageIDFromName(team)
{
	var index = data.team_pages[team]
	return GetTeamPageIDFromIndex(index)
}

function GetTeamPageIDFromIndex(index)
{
	return 'teampage_{0}'.format(index)
}

function ShowTeamPage(player_id)
{
	var page = data.team_pages[player_id]
    for (var i = 0; i < data.team_count; ++i)
	{
		var div = document.getElementById(GetTeamPageIDFromIndex(i))
		div.style.display = page == i ? "block" : "none"
	}
}
