var output
var ws
var parser

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

var team_pages;
var team_count = 0;

function GetTeamPageIDFromName(team)
{
	var index = team_pages[team]
	return GetTeamPageIDFromIndex(index)
}

function GetTeamPageIDFromIndex(index)
{
	return 'teampage_{0}'.format(index)
}

function ShowTeamPage(team)
{
	var page = team_pages[team]
    for (var i = 0; i < team_count; ++i)
	{
		var div = document.getElementById(GetTeamPageIDFromIndex(i))
		div.style.display = page == i ? "block" : "none"
	}
}
