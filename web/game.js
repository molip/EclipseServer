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
		ws = new WebSocket("ws://localhost:8998/echo");
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
